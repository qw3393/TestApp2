
// TestApp2Dlg.h: 헤더 파일
//

#pragma once


// CTestApp2Dlg 대화 상자
class CTestApp2Dlg : public CDialogEx
{
// 생성입니다.
public:
	CTestApp2Dlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTAPP2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMain();
	double GetFocusValue(Img2D o);
	char* UIstring(char* str);

	void* memCalloc_(dword siz, const char* pszFile, int32 nLine);
	void memFree_(void* ptr, const char* pszFile, int32 nLine);
	LRESULT auxMem_CBF(dword cmd, WPARAM wp, LPARAM lp, void* ptr);
	Img2D cv_imread(const TCHAR* pszname, int32 w, int32 h);
	boolean cv_imwrite(Img2D, const char* pszname, int32 w, int32 h);
	void cv_imread_test1();
	void cv_imwrite_test1();
	void ntg_filter_test();
	void ntg_filter_test2();
	void Img2D_FREE_TEST();
	MATRIX Mat_Load_s(char* pszName);
};
