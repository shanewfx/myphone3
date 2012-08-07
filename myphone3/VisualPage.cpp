///////////////////////////////////////////////////////////////////////////////
// VisualPage.cpp : implementation file for Visual/Video Settings Dialog class.
//
// MyPhone  -  Audio/Video IP telephony client.
// Compatible with H323 protocol, based on H323plus project.
// 
// Home Page: http://www.h323plus.org/
//
// Copyright (c) 2003 Michael Gerdov aka -=MaGGuS=-
// Copyright (c) 2007 ISVO (Asia) Pte Ltd.
//
// The contents of this file are subject to the Mozilla Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
// the License for the specific language governing rights and limitations
// under the License.
// 
// Thanks goes to All OpenH323/H323plus members (www.h323plus.org)
//				  and especially to Yuriy Gorvitovskiy (PocketBone).
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MyPhone.h"
#include "VisualPage.h"
//
#include "Multilang.h"		// Languages
//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//
const TCHAR OnCodecSuffix[] = _T(" (On)");
const TCHAR OffCodecSuffix[] = _T(" (Off)");

/////////////////////////////////////////////////////////////////////////////
// CVisualPage property page

IMPLEMENT_DYNCREATE(CVisualPage, CPropertyPage)

CVisualPage::CVisualPage() : CPropertyPage(CVisualPage::IDD)
{
	//{{AFX_DATA_INIT(CVisualPage)
	m_sendVideo = FALSE;
	m_receiveVideo = FALSE;
	m_RecordDeviceCur = _T("");
	m_localVideo = FALSE;
	m_LVflip = FALSE;
	m_RVflip = FALSE;
	m_vFPS = _T("");
	m_vINBPS = _T("");
        m_vOUTBPS = _T("");
	m_vQTY = _T("");
	m_RecDevSrc = -1;
	m_videoInSize = -1;
        m_videoOutSize = -1;
	m_localFlip = FALSE;
	//}}AFX_DATA_INIT
	m_showVideoSize = TRUE;

	this->m_psp.dwFlags^=PSP_HASHELP;
}

CVisualPage::~CVisualPage()
{
}

void CVisualPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RECORDING_COMBO, m_RecordDeviceBox);

	if (!pDX->m_bSaveAndValidate)
	{
		m_RecordDeviceBox.ResetContent();
		for (int i=0;i<m_RecordDeviceList.GetSize();i++)
			m_RecordDeviceBox.AddString(m_RecordDeviceList[i]);
	}	

	//{{AFX_DATA_MAP(CVisualPage)
	DDX_Control(pDX, IDC_VINBPSSPIN, m_videoInBPSspin);
	DDX_Control(pDX, IDC_VOUTBPSSPIN, m_videoOutBPSspin);
	DDX_Control(pDX, IDC_VFPSSPIN, m_videoFPSspin);
	DDX_Control(pDX, IDC_VQTYSPIN, m_videoQTYspin);
	DDX_Check(pDX, IDC_CHECK_SEND_VIDEO, m_sendVideo);
	DDX_Check(pDX, IDC_CHECK_RECEIVE_VIDEO, m_receiveVideo);
	DDX_Control(pDX, IDC_CS_UP_BUTTON, m_UpBtn);
	DDX_Control(pDX, IDC_CS_DOWN_BUTTON, m_DownBtn);
	DDX_Control(pDX, IDC_CS_ENABLED_CHECK, m_EnableBtn);
	DDX_Control(pDX, IDC_CODEC_SELECTION_LIST, m_CodecListCtrl);
	DDX_CBString(pDX, IDC_RECORDING_COMBO, m_RecordDeviceCur);
	DDX_Check(pDX, IDC_CHECK_LOCALVIDEO, m_localVideo);
	DDX_Check(pDX, IDC_LVFLIP, m_LVflip);
	DDX_Check(pDX, IDC_RVFLIP, m_RVflip);
	DDX_Text(pDX, IDC_VIDEOFPS, m_vFPS);
	DDX_Text(pDX, IDC_VIDEOINMAXBPS, m_vINBPS);
        DDX_Text(pDX, IDC_VIDEOOUTMAXBPS, m_vOUTBPS);
	DDX_Text(pDX, IDC_VIDEOQTY, m_vQTY);
	DDX_CBIndex(pDX, IDC_VIDDEVSRC_COMBO, m_RecDevSrc);
	DDX_CBIndex(pDX, IDC_VIDEOINSIZE, m_videoInSize);
        DDX_CBIndex(pDX, IDC_VIDEOOUTSIZE, m_videoOutSize);
	DDX_Check(pDX, IDC_LOCFLIPCHECK, m_localFlip);
	//}}AFX_DATA_MAP
	if (!pDX->m_bSaveAndValidate)
	{
		m_CodecListCtrl.ResetContent();
		for (int i=0;i<m_CodecList.GetSize();i++)
			m_CodecListCtrl.AddString(m_CodecList[i]);
	}
	else
	{
		m_CodecList.RemoveAll();
		for (int i=0;i<m_CodecListCtrl.GetCount();i++)
		{
			CString value;
			m_CodecListCtrl.GetText(i,value);
			m_CodecList.Add(value);
		}
	}	
}


BEGIN_MESSAGE_MAP(CVisualPage, CPropertyPage)
	//{{AFX_MSG_MAP(CVisualPage)
	ON_LBN_SELCHANGE(IDC_CODEC_SELECTION_LIST, OnSelchangeCodecSelectionList)
	ON_BN_CLICKED(IDC_CS_UP_BUTTON, OnCsUpButton)
	ON_BN_CLICKED(IDC_CS_DOWN_BUTTON, OnCsDownButton)
	ON_BN_CLICKED(IDC_CS_ENABLED_CHECK, OnCsEnabledCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualPage message handlers

BOOL CVisualPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_RecordDeviceBox.EnableWindow(TRUE);
//	GetDlgItem(IDC_VIDEOINSIZE)->EnableWindow(m_showVideoSize);
//	GetDlgItem(IDC_VIDEOOUTSIZE)->EnableWindow(m_showVideoSize);
//	GetDlgItem(IDC_VSIZE)->EnableWindow(m_showVideoSize);
	OnSelchangeCodecSelectionList();

	m_videoQTYspin.SetBuddy(GetDlgItem(IDC_VIDEOQTY));
	m_videoQTYspin.SetRange(1, 31);
	m_videoQTYspin.SetPos(m_videoQTY);
	m_videoInBPSspin.SetBuddy(GetDlgItem(IDC_VIDEOINMAXBPS));
	m_videoInBPSspin.SetRange(1, 10240);
        m_videoInBPSspin.SetPos(m_videoInMaxBPS);
        m_videoOutBPSspin.SetBuddy(GetDlgItem(IDC_VIDEOOUTMAXBPS));
        m_videoOutBPSspin.SetRange(1, 10240);	
	m_videoOutBPSspin.SetPos(m_videoOutMaxBPS);
	m_videoFPSspin.SetBuddy(GetDlgItem(IDC_VIDEOFPS));
	m_videoFPSspin.SetRange(1, 30);
	m_videoFPSspin.SetPos(m_videoFPS);
	//
	TranslateDlg();
	//
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVisualPage::OnSelchangeCodecSelectionList() 
{
	int selection = m_CodecListCtrl.GetCurSel();
	int count = m_CodecListCtrl.GetCount();

	m_UpBtn.EnableWindow(selection > 0 && selection < count);
	m_DownBtn.EnableWindow(selection >= 0 && selection < count-1);
	m_EnableBtn.EnableWindow(selection >= 0 && selection < count);

	CString value;
	if (selection >= 0 && selection < count)
	{
		m_CodecListCtrl.GetText(selection,value);
		m_EnableBtn.SetCheck(value.Find(OffCodecSuffix)==-1);
	}
	else
	{
		m_EnableBtn.SetCheck(0);
	}
}

void CVisualPage::OnCsUpButton() 
{
	int selection = m_CodecListCtrl.GetCurSel();
	if (selection<=0) return;
	CString value;
	m_CodecListCtrl.GetText(selection,value);

	m_CodecListCtrl.InsertString(selection-1,value);
	m_CodecListCtrl.DeleteString(selection+1);
	m_CodecListCtrl.SetCurSel(selection-1);

	OnSelchangeCodecSelectionList(); 
}

void CVisualPage::OnCsDownButton() 
{
	int selection = m_CodecListCtrl.GetCurSel();
	int count = m_CodecListCtrl.GetCount();
	if (selection<0 && selection>=count-1) return;

	CString value;
	m_CodecListCtrl.GetText(selection,value);

	m_CodecListCtrl.InsertString(selection+2,value);
	m_CodecListCtrl.DeleteString(selection);
	m_CodecListCtrl.SetCurSel(selection+1);

	OnSelchangeCodecSelectionList(); 
}

void CVisualPage::OnCsEnabledCheck() 
{
	int selection = m_CodecListCtrl.GetCurSel();	
	if (selection<0) return;

	CString value;
	m_CodecListCtrl.GetText(selection,value);

	if (m_EnableBtn.GetCheck())
		value.Replace(OffCodecSuffix, OnCodecSuffix);
	else
		value.Replace(OnCodecSuffix, OffCodecSuffix);

	m_CodecListCtrl.DeleteString(selection);
	m_CodecListCtrl.InsertString(selection,value);
	m_CodecListCtrl.SetCurSel(selection);
}

void CVisualPage::OnOK() 
{
	m_videoQTY = m_videoQTYspin.GetPos();
	m_videoInMaxBPS = m_videoInBPSspin.GetPos();
        m_videoOutMaxBPS = m_videoOutBPSspin.GetPos();
	m_videoFPS = m_videoFPSspin.GetPos();
	CPropertyPage::OnOK();
}

void CVisualPage::TranslateDlg()
{	// changing window languge
	GetDlgItem(IDC_VDEVSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_DEVSTR));
	GetDlgItem(IDC_VDEVCHSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_DEVCHSTR));
	GetDlgItem(IDC_CHECK_SEND_VIDEO)->SetWindowText((LPCTSTR)LoadStringLang(IDS_VSENDSTR));
	GetDlgItem(IDC_CHECK_RECEIVE_VIDEO)->SetWindowText((LPCTSTR)LoadStringLang(IDS_VRCVDSTR));
	GetDlgItem(IDC_CHECK_LOCALVIDEO)->SetWindowText((LPCTSTR)LoadStringLang(IDS_VLOCSTR));
	GetDlgItem(IDC_VIEWSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_VIEWSTR));	
	GetDlgItem(IDC_LVFLIP)->SetWindowText((LPCTSTR)LoadStringLang(IDS_LVFLIPSTR));	
	GetDlgItem(IDC_LOCFLIPCHECK)->SetWindowText((LPCTSTR)LoadStringLang(IDS_LOCFLIPSTR));	
	GetDlgItem(IDC_RVFLIP)->SetWindowText((LPCTSTR)LoadStringLang(IDS_RVFLIPSTR));
	GetDlgItem(IDC_QTYSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_QTYSTR));
	GetDlgItem(IDC_BPSSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_MBPSSTR));
//	GetDlgItem(IDC_FPSSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_FPSSTR));
	GetDlgItem(IDC_CSSTATIC)->SetWindowText((LPCTSTR)LoadStringLang(IDS_CODECSSTR));	
	GetDlgItem(IDC_CS_UP_BUTTON)->SetWindowText((LPCTSTR)LoadStringLang(IDS_CSUPSTR));	
	GetDlgItem(IDC_CS_ENABLED_CHECK)->SetWindowText((LPCTSTR)LoadStringLang(IDS_CSENBLSTR));	
	GetDlgItem(IDC_CS_DOWN_BUTTON)->SetWindowText((LPCTSTR)LoadStringLang(IDS_CSDOWNSTR));

//	SetWindowText((LPCTSTR)LoadStringLang(IDS_AUDIOSTR));	
}