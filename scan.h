#include <Windows.h>
#include <cfgmgr32.h>
#include <SetupAPI.h>
#include <vector>
#include <string>
#include <Usbioctl.h>
#include <list>
#include <atlstr.h>
#include <Usbiodef.h>
#include <fstream>
#pragma comment(lib, "setupapi.lib")
using namespace std;

bool scanPaused = false;																	//флаг остановки/запуска сканирования
int ejectCount = 3;																			//количество попыток отключения 


//
// USB info class
//
class DeviceInfo
{
public:
	std::wstring deviceHardwareId;															//ID USB-устройств
	std::wstring deviceFriendlyName;														//дружественное имя устройства
	std::wstring deviceObjectName;															//имя объекта
	std::wstring deviceDesc;																//описание устройства
	std::wstring deviceEnum;																//перечислитель устройства													
	std::wstring deviceProducer;															//производитель устройства
	bool available;																			//доступность устройства
	bool removable;																			//внешние уст-ва
	bool ejectSupported;																	//поддержка извлечения
	int deviceNumber;																		//номер устройства
	DEVINST deviceInst;																		//структура по которой к устройству можно получить доступ в системе
	bool currentlyAdded;																	//метка о том что устройство свежедобавленное
	char driveLetter;																		//буква устройства если есть
	ULARGE_INTEGER totalLogicalSpace;														//количество логической памяти	
	ULARGE_INTEGER freeLogicalSpace;														//свободная память
	ULARGE_INTEGER usedLogicalSpace;														//занятая память

	bool operator == (DeviceInfo& info){
		return deviceInst == info.deviceInst && deviceDesc == info.deviceDesc;
	}
	bool operator != (DeviceInfo& info){
		return ! operator == (info);
	}
};

class StorageInfo
{
public:
	ULARGE_INTEGER totalLogicalSpace;														//количество логической памяти
	LARGE_INTEGER totalPhysicalSpace;														//количество физической памяти
	ULARGE_INTEGER freeLogicalSpace;														//свободная память
	ULARGE_INTEGER usedLogicalSpace;														//занятая память
	ATL::CString serialNumber;																//серийный номер
	ATL::CString version;																	//версия
	ATL::CString firmware;																	//версия прошивки
	std::string connectingInterface;														//интерфейс
	bool usingPIO;																			//использование PIO
};

//
// функция получения списка подключенных USB-устройств
//
std::vector<DeviceInfo> GetUSBDevices()														//получение USB-устройств
{

	std::vector<DeviceInfo> result;															//итоговый список устройств


	const GUID* guid = &GUID_DEVINTERFACE_DISK;												//семейство системных имен
	
	//
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, L"USB", NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES);		//получаем хендл на выборку устройств по guid содержащих слово USB, сейчас находящихся в системе, всех классов


	DWORD dwIndex = 0;																		//индекс устройств в выборке
	long res = 1;																			//результат работы функций

	BYTE Buf[1024];																			//строка, в которую возвращается ответ от системы
	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;		//структура. в которой лежит детальная информация по интерфейсом устройства
	SP_DEVICE_INTERFACE_DATA         spdid;													//данные об интерфейсе
	SP_DEVINFO_DATA                  spdd;													//данные об устройстве
	spdd.cbSize = sizeof(SP_DEVINFO_DATA); 
	DWORD                            dwSize;												//размер данны=х который вернула функция
	spdid.cbSize = sizeof(spdid);

	
	while(true){

			spdid.cbSize = sizeof(spdid);													
			if (!SetupDiEnumDeviceInfo(hDevInfo, dwIndex, &spdd))							//если список устройст кончился - выйти
			{
				break;
			}
			dwIndex++;

			DWORD dwBytesReturned = 0;														
			DeviceInfo info;
			DWORD propType = 0;
			DWORD requiredSize = 0;
			WCHAR propBuffer[512];															//ответ от системы текстом

			
			info.deviceInst = spdd.DevInst;													//структура для обращения к устройству
																							//получение свойств устройства
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_MFG, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceProducer = propBuffer;
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_ENUMERATOR_NAME, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceEnum = propBuffer;
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_HARDWAREID, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceHardwareId = propBuffer;
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_FRIENDLYNAME, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceFriendlyName = propBuffer;
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceObjectName = propBuffer;
			SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_DEVICEDESC, &propType, (BYTE*)propBuffer, 512, &requiredSize);
			info.deviceDesc = propBuffer;
			DWORD flags = 0;
			//SetupDiGetDeviceRegistryProperty(hDevInfo, &spdd, SPDRP_CAPABILITIES , &propType, (BYTE*)&flags, 512, &requiredSize);
			info.ejectSupported = true;														
			info.removable = true;
			info.currentlyAdded = false;
			info.driveLetter = '/';															//буква диска
			ULARGE_INTEGER zero;															//константный ноль для инициализации всех размеров
			zero.QuadPart = 0;
			info.freeLogicalSpace = zero;
			info.totalLogicalSpace = zero;
			info.usedLogicalSpace = zero;
			result.push_back(info);

	}
	SetupDiDestroyDeviceInfoList(hDevInfo);													//уничтожить полученный от системы список устройств

	return result;
}


//
// вектор USB-устройств 
// 
std::vector<DeviceInfo> prevList = GetUSBDevices();											//полученный и выведенный список усб устройств
std::vector<DeviceInfo> permanentList = GetUSBDevices();									//список внутренних усб усройств

//
// функция отключения USB-устройств
//
BOOL EjectDevice(DeviceInfo& info)
{
	if(info.ejectSupported)																	//если устройство можно отклчить
	{
		DEVINST DevInst = info.deviceInst;													

		PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;										//неопределенный тип устройств
		WCHAR VetoNameW[MAX_PATH];															//имя типа устройств
		VetoNameW[0] = 0;																	
		bool bSuccess = false;																//успешность выполнения функции

		DEVINST DevInstParent = 0;															//родитель устройства
		DWORD res = CM_Get_Parent(&DevInstParent, DevInst, 0);								//получаем родителя устройства
		
		for(int tries = 1; tries <= ejectCount; tries++)
		{ 
			VetoType = PNP_VetoTypeUnknown;													//тип неизвестен
			VetoNameW[0] = 0;

			res = CM_Request_Device_EjectW( DevInst, &VetoType, VetoNameW, MAX_PATH, 0);	//запрашиваем извлечение устройства
			bSuccess = ( res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);				//извлеклось ли устройство
			if ( bSuccess )  { 
				break;
			}

			res = CM_Request_Device_EjectW( DevInstParent, &VetoType, VetoNameW, MAX_PATH, 0);		//запрашиваем извлечение устройства у его родителя
			bSuccess = ( res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);				//извлеклось ли устройство
			if ( bSuccess )  { 
				break;
			}

			Sleep(500); 
		}

		if ( bSuccess ) {
			return 0;
		}

	}
	return 1;
}

string getBus(DWORD buffer)														//перевод типа интерфейса из числа в строку
{
	string temp;
	switch (buffer)
	{
	
	case 0:
		temp = "Bus Type Unknown\n";
		return temp;
	case 1:
		temp = "Bus Type Scsi\n";
		return temp;
	case 2:
		temp = "Bus Type Atapi        \n";
		return temp;
	case 3:
		temp = "Bus Type Ata          \n";
		return temp;
	case 4:
		temp = "Bus Type 1394         \n";
		return temp;
	case 5:
		temp = "Bus Type Ssa          \n";
		return temp;
	case 6:
		temp = "Bus Type Fibre        \n";
		return temp;
	case 7:
		temp = "Bus Type Usb          \n";
		return temp;
	case 8:
		temp = "Bus Type RAID         \n";
		return temp;
	case 9:
		temp = "Bus Type iSCSI        \n";
		return temp;
	case 10:
		temp = "Bus Type Sas          \n";
		return temp;
	case 11:
		temp = "Bus Type Sata         \n";
		return temp;
	case 12:
		temp = "Bus Type Max Reserved  \n";
		return temp;
	default:
		break;
	}
}

//
//функция получения запоминающих устройств
//
std::vector<StorageInfo> getStorages()																
{
	std::vector<StorageInfo> currentList;																//текущий список запоминающих устройств для сохранения
	HDEVINFO hDeinfo;																					//дескриптор информации об устройстве
	BYTE Buf[1024];																						//буфер для возвращаемых данных
	SP_DEVICE_INTERFACE_DATA dataDeviceInterface;														//информация об интерфейсе
	PSP_DEVICE_INTERFACE_DETAIL_DATA interfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;		//детализированая информация об интефейсе
	SP_DEVINFO_DATA infoData;																			
	SP_DEVINFO_DATA deviceInfoData;																		
	DISK_GEOMETRY_EX diskGeometry;																		//геометрия диска
	DWORD sizeDeviceDetail = 0;																			//размер детализированой информации об интефейсе
	DWORD bufferSize = 0;																				//размер возвращаемого буфера
	STORAGE_PROPERTY_QUERY storagePropertyQuery;														//запрос свойств запоминающего устройства
	STORAGE_PROPERTY_QUERY adapterPropertyQuery;														//запрос свойств интерфейса
	STORAGE_ADAPTER_DESCRIPTOR storageAdapterDescriptor;												//дескриптор адаптера запоминающего устройства (используется для получения доступности PIO)
	LPTSTR propertyBuffer = NULL;																		//возвращаемый буфер для свойств
	DWORD propertySize = 0;																				
	STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };											
	DWORD dwBytesReturned = 0;
	CString strSerialNumber;																			//строка с серийным номером
	CString strVersion;																					//строка с версией
	CString strWar;																						//строка с прошивкой
	DWORD dwSerialNumberOffset;																			//смещение для серийного номера
	DWORD vers;																							//смещение для версии
	DWORD firmWare;																						//смещение для прошивки
	ULARGE_INTEGER freeSpace, totalSpace, junk, totalFreeSpace, totalAllSpace;							//переменные для размеров диска
	char disks[256];																					//список всех дисков
	char *disk;																							//указатель на текущую букву диска
	DWORD sizebuf = 256;

	totalFreeSpace.QuadPart = 0;
	totalAllSpace.QuadPart = 0;

	dataDeviceInterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));									//обнуление структуры
	storagePropertyQuery.PropertyId = StorageDeviceProperty;											//запрос на информацию об устройстве
	storagePropertyQuery.QueryType = PropertyStandardQuery;												//тип запроса не расширенный (стандартный)
	ZeroMemory(&adapterPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
	adapterPropertyQuery.PropertyId = StorageAdapterProperty;											//запрос на информацию об адаптере
	adapterPropertyQuery.QueryType = PropertyStandardQuery;

	hDeinfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);					//получить список устройств по этму гуиду с параметрами инерфейс и есть сейчас в системе
	
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDeinfo, NULL, &GUID_DEVINTERFACE_DISK, i, &dataDeviceInterface); i++)			//пока могу получить новое устройство
	{
		SetupDiGetInterfaceDeviceDetail(hDeinfo, &dataDeviceInterface, NULL, 0, &sizeDeviceDetail, NULL);						//получить размер детальной информации об устройстве
		
		
		interfaceDetailData->cbSize = sizeof(*interfaceDetailData);						
		ZeroMemory(&infoData, sizeof(infoData));
		infoData.cbSize = sizeof(infoData);

		long res = SetupDiGetInterfaceDeviceDetail(hDeinfo, &dataDeviceInterface, interfaceDetailData, sizeDeviceDetail, &sizeDeviceDetail, &infoData);	//получить детальную информацию об устройстве
		if (res)
		{
			
			HANDLE hDrive = CreateFile(interfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);		//получаю хендл на диск
			
			
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//полчаю размер данных об устройстве
				&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
				&dwBytesReturned, NULL);
			const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
			BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
			ZeroMemory(pOutBuffer, dwOutBufferSize);
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//получаю данные о запоминающем устройстве
				&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				pOutBuffer, dwOutBufferSize,
				&dwBytesReturned, NULL);
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//полчаю данные об адаптере, к которому подключено устройство
				&adapterPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				&storageAdapterDescriptor, sizeof(STORAGE_ADAPTER_DESCRIPTOR),
				&dwBytesReturned, NULL);

			DeviceIoControl(hDrive,  IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,						//получить геометрию диска
				NULL,0,
				&diskGeometry, sizeof(DISK_GEOMETRY_EX),
				&dwBytesReturned, NULL);
																								//запоминаю данные об устройстве
			STORAGE_DEVICE_DESCRIPTOR* pDeviceDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;	
			dwSerialNumberOffset = pDeviceDescriptor->SerialNumberOffset;
			vers = pDeviceDescriptor->Version; 
			firmWare = pDeviceDescriptor->ProductRevisionOffset; 
			if (dwSerialNumberOffset != 0)
			{
				strSerialNumber = CString(pOutBuffer + dwSerialNumberOffset+5);
				strVersion = CString(pOutBuffer + vers);
 				strWar = CString(pOutBuffer + firmWare);
			}
			GetLogicalDriveStrings(sizebuf, (LPWSTR)disks);										//получаю строку в которой список всех логических дисков
			disk = disks;
			while (*disk)																		//для каждого диска
			{
				if (GetDiskFreeSpaceEx((LPTSTR)disk, &junk, &totalSpace, &freeSpace))			//получить объем памяти и свободной памяти
				{					
					bool removable = false;
					if (GetDriveType((LPTSTR)disk) == DRIVE_REMOVABLE)							//если диск относится к извлекаемым
					{
						for (int t = 0; t < prevList.size(); t++)								//проверить чтобы он никому не был назначен
							if (prevList[t].driveLetter == disk[0]) removable = true;
						if (!removable)															//если не назначен, то найти свежедобавленное устройство и связать с диском
							for (int t = 0; t < prevList.size(); t++)
								if (prevList[t].currentlyAdded == true)
								{
									prevList[t].driveLetter = disk[0];
									prevList[t].totalLogicalSpace.QuadPart = totalSpace.QuadPart;
									prevList[t].freeLogicalSpace.QuadPart = freeSpace.QuadPart;
									prevList[t].usedLogicalSpace.QuadPart = prevList[t].totalLogicalSpace.QuadPart - prevList[t].freeLogicalSpace.QuadPart;
									prevList[t].currentlyAdded = false;
								}
					}
					else																		//если устройство неизвлекаемое, то добавить объем диска к текущему объему винчестера
					{
						totalFreeSpace.QuadPart += freeSpace.QuadPart;
						totalAllSpace.QuadPart += totalSpace.QuadPart;
					}
				}
				disk = disk + 8;																//перейти к следующему диску
			}

			StorageInfo currentStorage;															//записать считанную информацию

			currentStorage.totalLogicalSpace = totalAllSpace;
			currentStorage.totalPhysicalSpace = diskGeometry.DiskSize;
			currentStorage.freeLogicalSpace = totalFreeSpace;
			currentStorage.usedLogicalSpace.QuadPart = totalAllSpace.QuadPart - totalFreeSpace.QuadPart; 
			currentStorage.serialNumber = strSerialNumber.GetBuffer();
			currentStorage.version = strVersion;
			currentStorage.firmware = strWar;
			currentStorage.connectingInterface = getBus(pDeviceDescriptor->BusType);
			currentStorage.usingPIO = storageAdapterDescriptor.AdapterUsesPio;
			currentList.push_back(currentStorage);												//и добавить ее в список
			
		}
	}
	
	SetupDiDestroyDeviceInfoList(hDeinfo);														//уничтожить полученный от системы список устройств

	return currentList;										
}

std::vector<StorageInfo> storageList(50);														//список запоминающих устройств
