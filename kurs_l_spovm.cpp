// kurs_l_spovm.cpp: ������� ���� �������.

#include "stdafx.h"
#include "Form1.h"

using namespace kurs_l_spovm;

[STAThreadAttribute]
int main()
{
	// ��������� ���������� �������� Windows XP �� �������� �����-���� ��������� ����������
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// �������� �������� ���� � ��� ������
	Application::Run(gcnew Form1());
	return 0;
}
