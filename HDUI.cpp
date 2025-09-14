// HDUI.cpp: implementation of the CHDUI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FTTSP.h"
#include "HDUI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHDUI::CHDUI()
{

}

CHDUI::~CHDUI()
{

}

CString CHDUI::GetInstallPath()
{
	//!!28112019:LBs:HD testers need local install path -> removed get install path from config, return fixed path.
	return DEFAULT_TSTPRGM_PATH;
}

//Find the release used by Nextest for HD tester
CString CHDUI::NexTestRelease()
{
	CString NextestRelease;
	char  ValueName[200];
	char lpSubKey[1000]="SOFTWARE\\NexTest";
	DWORD dwcValueName = 200,retCode, retValue,j;

	HKEY hKey1;
	retCode = RegOpenKeyEx (HKEY_LOCAL_MACHINE, //Open the registry
							lpSubKey,
							 0,
							 KEY_ENUMERATE_SUB_KEYS |
							 KEY_EXECUTE |
							 KEY_QUERY_VALUE,
							 &hKey1);

	if(retCode == ERROR_SUCCESS) //Base opened
	{
		for (j = 0, retValue = ERROR_SUCCESS; retValue == ERROR_SUCCESS; j++)
		{
			dwcValueName = 128;
			ValueName[0] = '\0';
			unsigned char  bData[200];
			unsigned long cbData=200;
			retValue = RegEnumValue(hKey1, j, ValueName,
									&dwcValueName,
									NULL,
									NULL,		//&dwType
									bData,		//&bData
									&cbData);	//&bcData
			if(retValue == ERROR_SUCCESS)
			{
				if(strcmp(ValueName,"CurrentRelease")==0)
				{
					NextestRelease=bData;
					RegCloseKey (hKey1); 
					return NextestRelease;
				}
			}
		}
	}

	RegCloseKey (hKey1); 
	return "NOT_FOUND";
}

bool CHDUI::RetrieveCtrlAddress(CStringArray *Ip_Site)
{
	CFile myfile;
	CStringArray HsbNbre;
	CString buf,NumCtrl,AdrrCtrl;
	int n;

	if(!SetCurrentDirectory("C:\\TspFile\\"))
	{
		CString Message;
		Message.Format("Unable to access at directory : C:\\TspFile\\");
		AfxMessageBox(Message,MB_OK);
		return FALSE;
	}

	if(myfile.Open("CtrIpAddress.txt",CFile::modeRead))
	{
		CArchive ar(&myfile,CArchive::load);
		while(ar.ReadString(buf))
		{
			NumCtrl.Format("%s",buf);
			if(NumCtrl.Find("HSB") != -1) // to load only the site and not the ipc
			{
				n = NumCtrl.Find(" ",NumCtrl.Find("HSB"));
				NumCtrl=NumCtrl.Mid(n,NumCtrl.Find(')',n)-n);
				NumCtrl.Remove(' ');
				HsbNbre.Add(NumCtrl); //Number of controller
						
				AdrrCtrl.Format("%s",buf);
				n = AdrrCtrl.Find("10.1.1.");

				if(n != -1)
				{
					AdrrCtrl=AdrrCtrl.Mid(n,AdrrCtrl.Find(' ',n)-n);
					Ip_Site->Add(AdrrCtrl); //@ of controller
				}
				else
				{
					//simulation Mode
					n = AdrrCtrl.Find("127.0.0.");
					if(n != -1)
					{
						AdrrCtrl=AdrrCtrl.Mid(n,AdrrCtrl.Find(' ',n)-n);
						Ip_Site->Add(AdrrCtrl); //@ of controller
					}

				}
			}
		}
	}
	else
	{
		CString Message;
		Message.Format("Unable to open file  : C:\\TspFile\\CtrIpAddress.txt");
		AfxMessageBox(Message,MB_OK);
		return FALSE;
	}
	myfile.Close();

	//Sort the table if Controler number is not sorted
	int i=0,min,m;
	bool Flag;
	CString tmp;
	while(i<HsbNbre.GetSize())
	{
		Flag = false;
		min = atoi(HsbNbre.GetAt(i));
		for(int j = i; j<HsbNbre.GetSize() ; j++)
		{
			if(min > atoi(HsbNbre.GetAt(j)))
			{
				min = atoi(HsbNbre.GetAt(j));
				m=j;
				Flag=true;
			}
		}
		if(Flag)
		{
			tmp = HsbNbre.GetAt(i);
			HsbNbre.SetAt(i,HsbNbre.GetAt(m)); //Swap number of controler
			HsbNbre.SetAt(m,tmp);

			tmp = Ip_Site->GetAt(i);
			Ip_Site->SetAt(i,Ip_Site->GetAt(m)); //Swap @ of controler
			Ip_Site->SetAt(m,tmp);
		}
		i++;
	}
	return true;
}

CString CHDUI::GetUICmd(BOOL bEngiMode)
{
	CString theCmd;
	CString strCurRelease;

//	strCurRelease = GetCurrentRelease();
//	if (strCurRelease.IsEmpty())
//		return CString();

	strCurRelease=NexTestRelease(); //Find the release used
	if(operator !=(strCurRelease,"NOT_FOUND"))
		strCurRelease += "\\Utils\\";


	SetCurrentDirectory(strCurRelease);
	char IPcmd[512]="GetIpAddrs.exe > C:\\TspFile\\CtrIpAddress.txt";
	if(system(IPcmd)!=0)  
	{
		CString Message;
		Message.Format("GetIpAddrs.exe ERROR   : %s",IPcmd);
		AfxMessageBox(Message,MB_OK);
		return "";
	}

	//Retrieve the IP address for all sites controller
	CStringArray Ip_Site;
	if(!RetrieveCtrlAddress(&Ip_Site)) return "";

	if(m_nCtrls > Ip_Site.GetSize() )
	{
		CString Message;
		Message.Format("%d Controller to load and only %d Controller inside tester !!!",m_nCtrls,Ip_Site.GetSize() );
		AfxMessageBox(Message,MB_OK);
		return "";
	}



	//To know the controller to load
	if(strCurRelease.Find("h1.0")!=-1)
	{
		theCmd = "ui /NOLOGO /C=clear /CS=1 ";
		int i=1;
		CString Controler="";
		CString temp;
		do
		{			
			CString Id;
			Id.Format("%d ",i-1);// Id of Controller (i.e. 0,1,2...)
			CString NumCtrl = Ip_Site.GetAt(i-1);// address of controller (i.e. 10.1.1.4)
			temp="/C=\"";
			temp+=Id;
			Controler+=temp;
			Controler+=NumCtrl;
			Controler+="\" ";
			i++;
		}while(i<=m_nCtrls);
		theCmd += Controler;

	}
	else
	{
		theCmd = "ui /C=clear ";
		int i=1;
		CString Controler="/C=";
		do
		{			
			CString Tmp;
			Tmp.Format("1_%d",i);// Id of Controller (i.e. 0,1,2...)
			Controler+=Tmp;
			i++;
			if(i<=m_nCtrls)
				Controler+=",";
		}while(i<=m_nCtrls);
		theCmd += Controler;
		theCmd += " /NOLOGO /CS=1 ";

	}	

	theCmd += " /HT=40000 /MT=40000";

	if (bEngiMode)
		theCmd += " /E";

	theCmd += " \"" + m_InstalledProgramName + "\"";

	return theCmd;
}

/*
		if(ptrApp->ProbeInfo.NbrCtrl == 0)

		CString CurRelease;
		CurRelease=NexTestRelease(); //Find the release used
		if(operator !=(CurRelease,"NOT_FOUND"))
			CurRelease += "\\Utils\\";
		else 
		{
			CString Message;
			if(LanguageToUse=="FR")
				Message.Format("Impossible de lire la release de Nextest  : %s",CurRelease);
			else
				Message.Format("Unable to read the Nextest Release  : %s",CurRelease);
			AfxMessageBox(Message,MB_OK);
			return FALSE;
		}

		SetCurrentDirectory(CurRelease);
		char IPcmd[512]="GetIpAddrs.exe > C:\\TspFile\\CtrIpAddress.txt";
		if(system(IPcmd)!=0)  
		{
			CString Message;
			if(LanguageToUse=="FR")
				Message.Format("GetIpAddrs.exe ERREUR   : %s",IPcmd);
			else
				Message.Format("GetIpAddrs.exe ERROR   : %s",IPcmd);
			AfxMessageBox(Message,MB_OK);
			return FALSE;
		}

		//Retrieve the IP address for all sites controller
		CStringArray Ip_Site;
		if(!RetrieveCtrlAddress(&Ip_Site)) return false;

		if(ptrApp->ProbeInfo.NbrCtrl > Ip_Site.GetSize() )
		{
			CString Message;
			if(LanguageToUse=="FR")
				Message.Format("%d controleurs  charger pour %d controleurs dans le testeur !!!",ptrApp->ProbeInfo.NbrCtrl,Ip_Site.GetSize());
			else
				Message.Format("%d Controller to load and only %d Controller inside tester !!!",ptrApp->ProbeInfo.NbrCtrl,Ip_Site.GetSize() );
			AfxMessageBox(Message,MB_OK);
			return FALSE;
		}

		//To know the controller to load
		if(CurRelease.Find("h1.0")!=-1)
		{
			strcpy(UICmd,"ui /NOLOGO /C=clear /CS=1 ");
			int i=1;
			CString Controler="";
			CString temp;
			do
			{			
				CString Id;
				Id.Format("%d ",i-1);// Id of Controller (i.e. 0,1,2...)
				CString NumCtrl = Ip_Site.GetAt(i-1);// address of controller (i.e. 10.1.1.4)
				temp="/C=\"";
				temp+=Id;
				Controler+=temp;
				Controler+=NumCtrl;
				Controler+="\" ";
				i++;
			}while(i<=ptrApp->ProbeInfo.NbrCtrl);
			strcat(UICmd,Controler);

		}
		else
		{
			strcpy(UICmd,"ui /C=clear ");
			int i=1;
			CString Controler="/C=";
			do
			{			
				CString Tmp;
				Tmp.Format("1_%d",i);// Id of Controller (i.e. 0,1,2...)
				Controler+=Tmp;
				i++;
				if(i<=ptrApp->ProbeInfo.NbrCtrl)
					Controler+=",";
			}while(i<=ptrApp->ProbeInfo.NbrCtrl);
			strcat(UICmd,Controler);
			strcat(UICmd," /NOLOGO /CS=1 ");

		}	

*/