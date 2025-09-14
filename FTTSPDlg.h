// FTTSPDlg.h : header file
//

#if !defined(AFX_FTTSPDLG_H__CF689B35_61CE_4724_8391_F38569ABC346__INCLUDED_)
#define AFX_FTTSPDLG_H__CF689B35_61CE_4724_8391_F38569ABC346__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SessionConfiguration.h"

/////////////////////////////////////////////////////////////////////////////
// CFTTSPDlg dialog

class CFTTSPDlg : public CDialog
{
// Construction
public:
	CFTTSPDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFTTSPDlg(void);

// Dialog Data
	//{{AFX_DATA(CFTTSPDlg)
	enum { IDD = IDD_FTTSP_DIALOG };
	CButton	m_cRTM;
	CButton	m_EngiFrame;
	CStatic	m_Mode;
	CButton	m_ButtonCancel;
	CEdit	m_EditSubLotNumber;
	CEdit	m_szOperatorName;
	CEdit	m_szLoadBoardNumber;
	CEdit	m_szCableNumber;
	CComboBox	m_ComboCable;
	CComboBox	m_ComboLB;
	CComboBox	m_ComboRev;
	CComboBox	m_ComboHandlerDll;
	CComboBox	m_ComboSockets;
	CComboBox	m_ComboControllers;
	CButton	m_ButtonDegraded;
	CEdit	m_szHandlerId;
	CEdit	m_EditLotNumber;
	CComboBox	m_ComboHandler;
	CComboBox	m_ComboFlow;
	CComboBox	m_ComboPartID;
	CButton	m_ButtonStartProbe;
	CComboBox	m_ComboProduct;
	CString	m_szMode;
	CString	m_szTesterType;
	BOOL	m_bEngiOptCheck;
	CString	m_EngiTestPrgm;
	BOOL	m_bRTM;
	//}}AFX_DATA
	CSessionConfiguration m_SessionConfig;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTTSPDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL PreTranslateMessage(MSG* pMsg);
	void ShowHideEngiOptions(BOOL bShowHide);
	CString m_sPhoenixPath;
	void ResetSocketCheckBox();
	int FillComboWithFilenames(CComboBox& aCB, LPCSTR aPath, LPCSTR aFilter, BOOL bRemoveFilter=TRUE);
	void EnableCombo(CComboBox& aCB, BOOL bEnabled=TRUE);
	void UpdateControls(void);
	void ClearConfigurations( void );
	BOOL GetConfiguration(CConfiguration &aConfig);

	void EmptyMap(CMapStringToOb &aMap);
	void FillMap(
			const CObList &ConfigList,
			CMapStringToOb &MapToFill,
			CString (CConfiguration::*WhatToLookAt) ( void ) const
			);

	void FillCombo(CComboBox &aCB, CMapStringToOb &aMap);
	BOOL CheckGatewayData(CSetupGateway * pSubGateway);
	int FillProducts(void);
	int FillProductRev(void);
	int FillPartID(void);
	int FillFlow(void);
	int FillHandler(void);
	int FillHandlerDll(void);
	int FillLoadBoard(void);
	int FillCable(void);
	int FillControllersAndSockets(void);
	int GetSiteUsed(void);

	BOOL CheckForStart( void );
	int FilterForCurrentRelease(void);
	BOOL CheckSpecificRoutineForSubcontractor(CString &szSubcontractor);

	CObList m_ConfigurationList;
	CMapStringToOb m_MapRev;
	CMapStringToOb m_MapPartID;
	CMapStringToOb m_MapFlow;
	CMapStringToOb m_MapHandler;
	CMapStringToOb m_MapLoadBoardRef;
	CMapStringToOb m_MapCableRef;
	CMapStringToOb m_MapTesterType;
	CString szSetupRev;
	__int64 m_iRefCKS;
					
	HICON m_hIcon;

	CToolTipCtrl* m_pToolTip;

	// Generated message map functions
	//{{AFX_MSG(CFTTSPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeProduct();
	afx_msg void OnSelchangePartId();
	afx_msg void OnSelchangeFlow();
	afx_msg void OnSelchangeHandler();
	afx_msg void OnChangeHandlerId();
	afx_msg void OnChangeLotNumber();
	afx_msg void OnDegradedMode();
	afx_msg void OnSelchangeHandlerDll();
	afx_msg void OnSelchangeProductRev();
	afx_msg void OnChangeLbNumber();
	afx_msg void OnChangeCableNumber();
	afx_msg void OnChangeOpName();
	afx_msg void OnChangeSubLotNumber();
	afx_msg void OnCheckSite();
	afx_msg void OnCheckAllSites();
	virtual void OnOK();
	afx_msg void OnCheckEngiOpt();
	afx_msg void OnBrowseTpgrm();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTTSPDLG_H__CF689B35_61CE_4724_8391_F38569ABC346__INCLUDED_)
