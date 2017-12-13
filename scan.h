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

bool scanPaused = false;																	//���� ���������/������� ������������
int ejectCount = 3;																			//���������� ������� ���������� 


//
// USB info class
//
class DeviceInfo
{
public:
	std::wstring deviceHardwareId;															//ID USB-���������
	std::wstring deviceFriendlyName;														//������������� ��� ����������
	std::wstring deviceObjectName;															//��� �������
	std::wstring deviceDesc;																//�������� ����������
	std::wstring deviceEnum;																//������������� ����������													
	std::wstring deviceProducer;															//������������� ����������
	bool available;																			//����������� ����������
	bool removable;																			//������� ���-��
	bool ejectSupported;																	//��������� ����������
	int deviceNumber;																		//����� ����������
	DEVINST deviceInst;																		//��������� �� ������� � ���������� ����� �������� ������ � �������
	bool currentlyAdded;																	//����� � ��� ��� ���������� ����������������
	char driveLetter;																		//����� ���������� ���� ����
	ULARGE_INTEGER totalLogicalSpace;														//���������� ���������� ������	
	ULARGE_INTEGER freeLogicalSpace;														//��������� ������
	ULARGE_INTEGER usedLogicalSpace;														//������� ������

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
	ULARGE_INTEGER totalLogicalSpace;														//���������� ���������� ������
	LARGE_INTEGER totalPhysicalSpace;														//���������� ���������� ������
	ULARGE_INTEGER freeLogicalSpace;														//��������� ������
	ULARGE_INTEGER usedLogicalSpace;														//������� ������
	ATL::CString serialNumber;																//�������� �����
	ATL::CString version;																	//������
	ATL::CString firmware;																	//������ ��������
	std::string connectingInterface;														//���������
	bool usingPIO;																			//������������� PIO
};

//
// ������� ��������� ������ ������������ USB-���������
//
std::vector<DeviceInfo> GetUSBDevices()														//��������� USB-���������
{

	std::vector<DeviceInfo> result;															//�������� ������ ���������


	const GUID* guid = &GUID_DEVINTERFACE_DISK;												//��������� ��������� ����
	
	//
	HDEVINFO hDevInfo = SetupDiGetClassDevs(guid, L"USB", NULL, DIGCF_PRESENT|DIGCF_ALLCLASSES);		//�������� ����� �� ������� ��������� �� guid ���������� ����� USB, ������ ����������� � �������, ���� �������


	DWORD dwIndex = 0;																		//������ ��������� � �������
	long res = 1;																			//��������� ������ �������

	BYTE Buf[1024];																			//������, � ������� ������������ ����� �� �������
	PSP_DEVICE_INTERFACE_DETAIL_DATA pspdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;		//���������. � ������� ����� ��������� ���������� �� ����������� ����������
	SP_DEVICE_INTERFACE_DATA         spdid;													//������ �� ����������
	SP_DEVINFO_DATA                  spdd;													//������ �� ����������
	spdd.cbSize = sizeof(SP_DEVINFO_DATA); 
	DWORD                            dwSize;												//������ �����=� ������� ������� �������
	spdid.cbSize = sizeof(spdid);

	
	while(true){

			spdid.cbSize = sizeof(spdid);													
			if (!SetupDiEnumDeviceInfo(hDevInfo, dwIndex, &spdd))							//���� ������ �������� �������� - �����
			{
				break;
			}
			dwIndex++;

			DWORD dwBytesReturned = 0;														
			DeviceInfo info;
			DWORD propType = 0;
			DWORD requiredSize = 0;
			WCHAR propBuffer[512];															//����� �� ������� �������

			
			info.deviceInst = spdd.DevInst;													//��������� ��� ��������� � ����������
																							//��������� ������� ����������
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
			info.driveLetter = '/';															//����� �����
			ULARGE_INTEGER zero;															//����������� ���� ��� ������������� ���� ��������
			zero.QuadPart = 0;
			info.freeLogicalSpace = zero;
			info.totalLogicalSpace = zero;
			info.usedLogicalSpace = zero;
			result.push_back(info);

	}
	SetupDiDestroyDeviceInfoList(hDevInfo);													//���������� ���������� �� ������� ������ ���������

	return result;
}


//
// ������ USB-��������� 
// 
std::vector<DeviceInfo> prevList = GetUSBDevices();											//���������� � ���������� ������ ��� ���������
std::vector<DeviceInfo> permanentList = GetUSBDevices();									//������ ���������� ��� ��������

//
// ������� ���������� USB-���������
//
BOOL EjectDevice(DeviceInfo& info)
{
	if(info.ejectSupported)																	//���� ���������� ����� ��������
	{
		DEVINST DevInst = info.deviceInst;													

		PNP_VETO_TYPE VetoType = PNP_VetoTypeUnknown;										//�������������� ��� ���������
		WCHAR VetoNameW[MAX_PATH];															//��� ���� ���������
		VetoNameW[0] = 0;																	
		bool bSuccess = false;																//���������� ���������� �������

		DEVINST DevInstParent = 0;															//�������� ����������
		DWORD res = CM_Get_Parent(&DevInstParent, DevInst, 0);								//�������� �������� ����������
		
		for(int tries = 1; tries <= ejectCount; tries++)
		{ 
			VetoType = PNP_VetoTypeUnknown;													//��� ����������
			VetoNameW[0] = 0;

			res = CM_Request_Device_EjectW( DevInst, &VetoType, VetoNameW, MAX_PATH, 0);	//����������� ���������� ����������
			bSuccess = ( res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);				//���������� �� ����������
			if ( bSuccess )  { 
				break;
			}

			res = CM_Request_Device_EjectW( DevInstParent, &VetoType, VetoNameW, MAX_PATH, 0);		//����������� ���������� ���������� � ��� ��������
			bSuccess = ( res == CR_SUCCESS && VetoType == PNP_VetoTypeUnknown);				//���������� �� ����������
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

string getBus(DWORD buffer)														//������� ���� ���������� �� ����� � ������
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
//������� ��������� ������������ ���������
//
std::vector<StorageInfo> getStorages()																
{
	std::vector<StorageInfo> currentList;																//������� ������ ������������ ��������� ��� ����������
	HDEVINFO hDeinfo;																					//���������� ���������� �� ����������
	BYTE Buf[1024];																						//����� ��� ������������ ������
	SP_DEVICE_INTERFACE_DATA dataDeviceInterface;														//���������� �� ����������
	PSP_DEVICE_INTERFACE_DETAIL_DATA interfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)Buf;		//��������������� ���������� �� ���������
	SP_DEVINFO_DATA infoData;																			
	SP_DEVINFO_DATA deviceInfoData;																		
	DISK_GEOMETRY_EX diskGeometry;																		//��������� �����
	DWORD sizeDeviceDetail = 0;																			//������ ��������������� ���������� �� ���������
	DWORD bufferSize = 0;																				//������ ������������� ������
	STORAGE_PROPERTY_QUERY storagePropertyQuery;														//������ ������� ������������� ����������
	STORAGE_PROPERTY_QUERY adapterPropertyQuery;														//������ ������� ����������
	STORAGE_ADAPTER_DESCRIPTOR storageAdapterDescriptor;												//���������� �������� ������������� ���������� (������������ ��� ��������� ����������� PIO)
	LPTSTR propertyBuffer = NULL;																		//������������ ����� ��� �������
	DWORD propertySize = 0;																				
	STORAGE_DESCRIPTOR_HEADER storageDescriptorHeader = { 0 };											
	DWORD dwBytesReturned = 0;
	CString strSerialNumber;																			//������ � �������� �������
	CString strVersion;																					//������ � �������
	CString strWar;																						//������ � ���������
	DWORD dwSerialNumberOffset;																			//�������� ��� ��������� ������
	DWORD vers;																							//�������� ��� ������
	DWORD firmWare;																						//�������� ��� ��������
	ULARGE_INTEGER freeSpace, totalSpace, junk, totalFreeSpace, totalAllSpace;							//���������� ��� �������� �����
	char disks[256];																					//������ ���� ������
	char *disk;																							//��������� �� ������� ����� �����
	DWORD sizebuf = 256;

	totalFreeSpace.QuadPart = 0;
	totalAllSpace.QuadPart = 0;

	dataDeviceInterface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	ZeroMemory(&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));									//��������� ���������
	storagePropertyQuery.PropertyId = StorageDeviceProperty;											//������ �� ���������� �� ����������
	storagePropertyQuery.QueryType = PropertyStandardQuery;												//��� ������� �� ����������� (�����������)
	ZeroMemory(&adapterPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY));
	adapterPropertyQuery.PropertyId = StorageAdapterProperty;											//������ �� ���������� �� ��������
	adapterPropertyQuery.QueryType = PropertyStandardQuery;

	hDeinfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_DISK, NULL, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);					//�������� ������ ��������� �� ���� ����� � ����������� �������� � ���� ������ � �������
	
	for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDeinfo, NULL, &GUID_DEVINTERFACE_DISK, i, &dataDeviceInterface); i++)			//���� ���� �������� ����� ����������
	{
		SetupDiGetInterfaceDeviceDetail(hDeinfo, &dataDeviceInterface, NULL, 0, &sizeDeviceDetail, NULL);						//�������� ������ ��������� ���������� �� ����������
		
		
		interfaceDetailData->cbSize = sizeof(*interfaceDetailData);						
		ZeroMemory(&infoData, sizeof(infoData));
		infoData.cbSize = sizeof(infoData);

		long res = SetupDiGetInterfaceDeviceDetail(hDeinfo, &dataDeviceInterface, interfaceDetailData, sizeDeviceDetail, &sizeDeviceDetail, &infoData);	//�������� ��������� ���������� �� ����������
		if (res)
		{
			
			HANDLE hDrive = CreateFile(interfaceDetailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);		//������� ����� �� ����
			
			
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//������ ������ ������ �� ����������
				&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				&storageDescriptorHeader, sizeof(STORAGE_DESCRIPTOR_HEADER),
				&dwBytesReturned, NULL);
			const DWORD dwOutBufferSize = storageDescriptorHeader.Size;
			BYTE* pOutBuffer = new BYTE[dwOutBufferSize];
			ZeroMemory(pOutBuffer, dwOutBufferSize);
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//������� ������ � ������������ ����������
				&storagePropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				pOutBuffer, dwOutBufferSize,
				&dwBytesReturned, NULL);
			DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,								//������ ������ �� ��������, � �������� ���������� ����������
				&adapterPropertyQuery, sizeof(STORAGE_PROPERTY_QUERY),
				&storageAdapterDescriptor, sizeof(STORAGE_ADAPTER_DESCRIPTOR),
				&dwBytesReturned, NULL);

			DeviceIoControl(hDrive,  IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,						//�������� ��������� �����
				NULL,0,
				&diskGeometry, sizeof(DISK_GEOMETRY_EX),
				&dwBytesReturned, NULL);
																								//��������� ������ �� ����������
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
			GetLogicalDriveStrings(sizebuf, (LPWSTR)disks);										//������� ������ � ������� ������ ���� ���������� ������
			disk = disks;
			while (*disk)																		//��� ������� �����
			{
				if (GetDiskFreeSpaceEx((LPTSTR)disk, &junk, &totalSpace, &freeSpace))			//�������� ����� ������ � ��������� ������
				{					
					bool removable = false;
					if (GetDriveType((LPTSTR)disk) == DRIVE_REMOVABLE)							//���� ���� ��������� � �����������
					{
						for (int t = 0; t < prevList.size(); t++)								//��������� ����� �� ������ �� ��� ��������
							if (prevList[t].driveLetter == disk[0]) removable = true;
						if (!removable)															//���� �� ��������, �� ����� ���������������� ���������� � ������� � ������
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
					else																		//���� ���������� �������������, �� �������� ����� ����� � �������� ������ ����������
					{
						totalFreeSpace.QuadPart += freeSpace.QuadPart;
						totalAllSpace.QuadPart += totalSpace.QuadPart;
					}
				}
				disk = disk + 8;																//������� � ���������� �����
			}

			StorageInfo currentStorage;															//�������� ��������� ����������

			currentStorage.totalLogicalSpace = totalAllSpace;
			currentStorage.totalPhysicalSpace = diskGeometry.DiskSize;
			currentStorage.freeLogicalSpace = totalFreeSpace;
			currentStorage.usedLogicalSpace.QuadPart = totalAllSpace.QuadPart - totalFreeSpace.QuadPart; 
			currentStorage.serialNumber = strSerialNumber.GetBuffer();
			currentStorage.version = strVersion;
			currentStorage.firmware = strWar;
			currentStorage.connectingInterface = getBus(pDeviceDescriptor->BusType);
			currentStorage.usingPIO = storageAdapterDescriptor.AdapterUsesPio;
			currentList.push_back(currentStorage);												//� �������� �� � ������
			
		}
	}
	
	SetupDiDestroyDeviceInfoList(hDeinfo);														//���������� ���������� �� ������� ������ ���������

	return currentList;										
}

std::vector<StorageInfo> storageList(50);														//������ ������������ ���������
