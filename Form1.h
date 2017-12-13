#pragma once
#include "scan.h"

// Scan interval
#define SCAN_DELAY 1000


namespace kurs_l_spovm {

	using namespace System;
	using namespace System::IO;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// Сводка для Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//		
			ListDevices();																	//создание списка USB-устройств
			ListStorages();																	//создание списка запоминающих устройств
			listBox1->SelectedIndex = 0;
			listBox2->SelectedIndex = 0;
			button4->BackColor = scanPaused?Color::Red:Color::Green;						//цвет кнопки если запущено или остановлено сканирование
			timer1->Interval = SCAN_DELAY;													//установка интервала таймера
			timer1->Start();																//запуск таймера
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}		
		}
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Label^  label4;

	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::ListBox^  listBox2;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Button^  button5;

	protected: 
	private: System::Windows::Forms::ListBox^  listBox1;
			 
	protected:  template<class T> bool Contains(std::vector<T>& vector, T value)			//проверка на наличие объекта в векторе
	{
	for(int i = 0; i < vector.size(); i++)
		if(vector[i] == value)
			return true;
	return false;
	}
	
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Timer^  timer1;
	private: System::ComponentModel::IContainer^  components;


	private:
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Обязательный метод для поддержки конструктора - не изменяйте
		/// содержимое данного метода при помощи редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->listBox1 = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->listBox2 = (gcnew System::Windows::Forms::ListBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->FormattingEnabled = true;
			this->listBox1->Location = System::Drawing::Point(12, 21);
			this->listBox1->Name = L"listBox1";
			this->listBox1->Size = System::Drawing::Size(318, 420);
			this->listBox1->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(336, 21);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(90, 16);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Connecting";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(336, 92);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(80, 16);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Removing";
			// 
			// timer1
			// 
			this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->label3->Location = System::Drawing::Point(9, 2);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(145, 16);
			this->label3->TabIndex = 3;
			this->label3->Text = L"List of USB devices";
			// 
			// button1
			// 
			this->button1->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->button1->Location = System::Drawing::Point(336, 173);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(185, 39);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Eject selected device";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(336, 420);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(100, 20);
			this->textBox1->TabIndex = 5;
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(445, 418);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(131, 23);
			this->button2->TabIndex = 6;
			this->button2->Text = L"Set eject attempt count";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->label4->Location = System::Drawing::Point(583, 420);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(118, 16);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Current EAC: 3 ";
			// 
			// button3
			// 
			this->button3->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->button3->Location = System::Drawing::Point(336, 219);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(185, 23);
			this->button3->TabIndex = 8;
			this->button3->Text = L"Show USB device info";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// listBox2
			// 
			this->listBox2->FormattingEnabled = true;
			this->listBox2->Location = System::Drawing::Point(337, 274);
			this->listBox2->Name = L"listBox2";
			this->listBox2->Size = System::Drawing::Size(360, 121);
			this->listBox2->TabIndex = 9;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->label5->Location = System::Drawing::Point(336, 255);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(122, 16);
			this->label5->TabIndex = 10;
			this->label5->Text = L"Storage devices";
			// 
			// button4
			// 
			this->button4->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->button4->Location = System::Drawing::Point(527, 173);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(169, 39);
			this->button4->TabIndex = 11;
			this->button4->Text = L"Start/stop scan";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::button4_Click);
			// 
			// button5
			// 
			this->button5->Font = (gcnew System::Drawing::Font(L"Verdana", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->button5->Location = System::Drawing::Point(528, 219);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(169, 23);
			this->button5->TabIndex = 12;
			this->button5->Text = L"Show storage info";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(709, 453);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->listBox2);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->listBox1);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		
	private: void ListDevices()																//вывод списка USB-устройств
	{
		listBox1->Items->Clear();
		if(prevList.size() == 0)
		{
			listBox1->Items->Add("No USB devices connected.");								//если нет подключеных USB-устройств
		}
		for(int i = 0; i < prevList.size(); i++)
		{	
			listBox1->Items->Add(gcnew String(prevList[i].deviceDesc.c_str()) + (Contains(permanentList, prevList[i]) ? "[Internal]" : "[External]"));	//добавление усб устройства в список усб устройств
		}
		listBox1->SelectedIndex = 0;

	}

	private: void ListStorages()															//получение и вывод списка запоминающих устройств
	{
		int prevSelectedIndex = listBox2->SelectedIndex;
		storageList.clear();																//очистка списка запоминающих устройств
		listBox2->Items->Clear();
		storageList=getStorages();															//получение списка запоминающих устройств
		
		if(storageList.size() == 0)
		{
			listBox2->Items->Add("No storage devices found.");								//если нет подключенных запоминающих устройств
		}
		for(int i = 0; i < storageList.size(); i++)
		{			
			listBox2->Items->Add(String::Concat(gcnew String("Device "),Convert::ToString(i+1),": ",gcnew String(storageList[i].version))); //добавить устройство в списокк запоминающих
		}
		listBox2->SelectedIndex = prevSelectedIndex < listBox2->Items->Count ? prevSelectedIndex : 0;
	}

	private: System::Void timer1_Tick(System::Object^  sender, System::EventArgs^  e)		//таймер в отдельном потоке для сканирования
	{
		if( !scanPaused ){
			std::vector<DeviceInfo> currentList = GetUSBDevices();	// текущий список устройств
			bool needToCheck = false;

			// поиск новых устройств
			if(currentList.size()!=prevList.size())
				needToCheck = true;
			else
				for(int i = 0; i < prevList.size(); i++)
					if (currentList[i]!=prevList[i])
					{
						needToCheck = true;
						break;
					}
			if (needToCheck)	// если список надо перепроверить
			{
				for(int i = 0; i < currentList.size(); i++){
					if( !Contains(prevList, currentList[i] ) ){	//если устройство было добавлено вывести лейбл на экран
						currentList[i].currentlyAdded = true;	//пометить как свежедобавленное
						label1->Text = String::Concat(gcnew String("The device has been connected:\n Name: ") ,gcnew String(currentList[i].deviceDesc.c_str()) ,gcnew String("\nPath:\n") ,gcnew String(currentList[i].deviceObjectName.c_str()));
						label1->BackColor = Color::Green;
						label1->Visible = true;
						label2->Visible = false;
					}
					else
						currentList[i].currentlyAdded = false; //иначе пометить как не свежедобавленное
				}

			// поиск удаленных устройств
				for(int i = 0; i < prevList.size(); i++){
					if( !Contains(currentList, prevList[i] ) ){	//если устройство исчесло из списка вывести на экран что устройство было удалено	
						label2->Text = String::Concat(gcnew String("The device has been removed:\n Name: ") ,gcnew String(prevList[i].deviceDesc.c_str()) ,gcnew String("\nPath:\n") ,gcnew String(prevList[i].deviceObjectName.c_str()));
						label2->BackColor = Color::Red;
						label2->Visible = true;
						label1->Visible = false;
					
					}
				}
				if (currentList.size() < permanentList.size()) permanentList = currentList; //обновить список внутренних устройств если оказалось что одно из первоначальноподключенных - не внутреннее
				for (int i = 0; i < prevList.size(); i++)
				{
					for (int j = 0; j < currentList.size(); j++)					
						if (prevList[i] == currentList[j])
						{
							bool tempCurrentlyAdded = currentList[j].currentlyAdded;
							currentList[j] = prevList[i];
							currentList[j].currentlyAdded = tempCurrentlyAdded;
						}
				}
				prevList = currentList;
				ListDevices();		
				
			}
			ListStorages();
		}
			 }
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {	//кнопка безопасно извлечения
				 if(EjectDevice(prevList[listBox1->SelectedIndex]))	System::Windows::Forms::MessageBox::Show("Can't eject for "+System::Convert::ToString(ejectCount)+" times");					
				 else  
				 {
					 System::Windows::Forms::MessageBox::Show("Device "+listBox1->SelectedItem+" succesfuly ejected");
					 ListDevices();
					 ListStorages();
				 }
			 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {	//изменение количества попыток извлечения
			 ejectCount = System::Convert::ToInt32(textBox1->Text)>0?System::Convert::ToInt32(textBox1->Text):3;
			 textBox1->Clear();
			 label4->Text = String::Concat(gcnew String("Current EAC: "), System::Convert::ToString(ejectCount));
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {	//вывести информацию о усб
			 System::Windows::Forms::MessageBox::Show("Device description: " +
				 gcnew String(prevList[listBox1->SelectedIndex].deviceDesc.c_str()) + "\n" +
				 "Device enumerator: "+ gcnew String(prevList[listBox1->SelectedIndex].deviceEnum.c_str()) + "\n" +
				 "Device friendly name: "+ gcnew String(prevList[listBox1->SelectedIndex].deviceFriendlyName.c_str()) + "\n" +
				 "Hardware ID: "+ gcnew String(prevList[listBox1->SelectedIndex].deviceHardwareId.c_str()) + "\n" +
				 "Device object name: "+ gcnew String(prevList[listBox1->SelectedIndex].deviceObjectName.c_str()) + "\n" +
				 "Device producer: "+ gcnew String(prevList[listBox1->SelectedIndex].deviceProducer.c_str()) + "\n"  +
				 "Logical space: " + Convert::ToString(prevList[listBox1->SelectedIndex].totalLogicalSpace.QuadPart) + " bytes\n" +
				 "Free logical space: " + Convert::ToString(prevList[listBox1->SelectedIndex].freeLogicalSpace.QuadPart) + " bytes\n" +
				 "Used logical space: " + Convert::ToString(prevList[listBox1->SelectedIndex].usedLogicalSpace.QuadPart) + " bytes\n" +
				 "Drive letter: " + Char(prevList[listBox1->SelectedIndex].driveLetter)
 			 );
		 }
private: System::Void button5_Click(System::Object^ sender, System::EventArgs^ e) { // вывести инфформацию о винчестере 
			System::Windows::Forms::MessageBox::Show("Physical space: " + 
				Convert::ToString(storageList[listBox2->SelectedIndex].totalPhysicalSpace.QuadPart) + " bytes\n" + 
				"Logical space: " + Convert::ToString(storageList[listBox2->SelectedIndex].totalLogicalSpace.QuadPart) + " bytes\n" + 
				"Free logical space: " + Convert::ToString(storageList[listBox2->SelectedIndex].freeLogicalSpace.QuadPart) + " bytes\n" + 
				"Used logical space: " + Convert::ToString(storageList[listBox2->SelectedIndex].usedLogicalSpace.QuadPart) + " bytes\n" + 
				"Interface: " + gcnew String(storageList[listBox2->SelectedIndex].connectingInterface.c_str()) + 
				"ATA: " + (storageList[listBox2->SelectedIndex].connectingInterface.compare("Bus Type Sata \n") ? "1" : "1-7") + "\n" + 
				"Firmware: " + gcnew String(storageList[listBox2->SelectedIndex].firmware) + "\n" + 
				"Serial Number: " + gcnew String(storageList[listBox2->SelectedIndex].serialNumber) + "\n" + 
				"Version: " + gcnew String(storageList[listBox2->SelectedIndex].version) + "\n" + 
				"Using DMA: " + (storageList[listBox2->SelectedIndex].connectingInterface.compare("Bus Type Sata \n") && storageList[listBox2->SelectedIndex].connectingInterface.compare("Bus Type Ata \n") ? "False" : addingDMA()) + "\n" + 
				"Using PIO: " + addingPIO(storageList[listBox2->SelectedIndex].usingPIO) 
); 
} 

System::String^ addingPIO(bool adding) 
{ 
	if (!adding) 
		return "False"; 
	else 
	return gcnew String("PIO Mode 0\n" + "PIO Mode 1\n" + "PIO Mode 2\n" + "PIO Mode 3\n" + "PIO Mode 4\n"); 
} 

String^ addingDMA() 
{ 
	return gcnew String("SW DMA Mode 0\n" + "SW DMA Mode 1\n" + "SW DMA Mode 2\n" + "MW DMA Mode 0\n" + "MW DMA Mode 1\n" + "MW DMA Mode 2\n" + 
"UDMA Mode 0\n" + "UDMA Mode 1\n" + "UDMA Mode 2\n" + "UDMA Mode 3\n" + "UDMA Mode 4\n" + "UDMA Mode 5\n" + "UDMA Mode 6\n"); 
}
		
private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {	//остановить или запустить сканирование
			 scanPaused = !scanPaused;
			 button4->BackColor = scanPaused?Color::Red:Color::Green;
			 if (scanPaused) timer1->Stop();
			 else timer1->Start();
			 ListDevices();
			 ListStorages();
		 }
};

}

