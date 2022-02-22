
// TestApp2Dlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "TestApp2.h"
#include "TestApp2Dlg.h"
#include "afxdialogex.h"
#include <string>
#include "filter//filter_main.h"

#include <iostream>
#include <fstream>		//ifstream
#include <vector>		//vector
#include <sstream>      // std::stringstream, std::stringbuf

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define UI_STRING_NUM (64)
#define UI_STRING_LEN (128)
#define memCalloc(siz)			memCalloc_(siz,__FILE__,__LINE__)
#define memFree(ptr)				memFree_(ptr,__FILE__,__LINE__)
#define AUXMEM_TBL_MAX (4)
#define AUXMEM_LIST_SIZE (512)

static MEMTBL_s* memTbl[AUXMEM_TBL_MAX];
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestApp2Dlg 대화 상자



CTestApp2Dlg::CTestApp2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTAPP2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestApp2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestApp2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_MAIN, &CTestApp2Dlg::OnBnClickedButtonMain)
END_MESSAGE_MAP()

void CTestApp2Dlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DBGCLOSE();
	CDialog::OnClose();
}
// CTestApp2Dlg 메시지 처리기

BOOL CTestApp2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	DBGINIT(NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestApp2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestApp2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestApp2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestApp2Dlg::OnBnClickedButtonMain()
{
	DBGPRINT("시작\n");
	//cv_imwrite_test1(); //간지검사PC _ imwrite 추가
	ntg_filter_test();
	
	//ntg_filter_test2(); NTG필터링


	// odi csv read test
	//TCHAR				str[128];
	//CStringA			temp;
	//mcxt				watch;
	//double64			elapse;
	////MATRIX				b;

	//watch = Time_STAlloc();
	//Time_STTick(watch);
	//MATRIX b = Mat_Load_s("D:\\Result\\pdm_test\\filtered_Area1.csv");

	////_stprintf(str, _T("D:\\Result\\pdm_test\\filtered_Area1.csv"));
	////temp = CStringA(str);
	////MATRIX b = Mat_Load("D:\\Result\\pdm_test\\filtered_Area1.csv");
	//Time_STTock(watch, &elapse);
	//DBGPRINT("hr_tm  = %f\n", elapse);
	//MAT_FREE(b);

	//odi matlab test
	//if (FileIo_IsFileExist(_T("D:\\Algorithm\\Tool_Test\\Algorithm"), _T("Tool_test.exe")) == TRUE)
	//{
	//	ShellExecute(NULL, _T("open"), _T("D:\\Algorithm\\Tool_Test\\Algorithm\\Tool_test.exe"), _T("D:\\Scandata\\LOTID_202108051224"), NULL, SW_HIDE);
	//	DBGPRINT("[MARK] Matlab executed.\n");
	//}
	//else
	//{
	//	DBGPRINT("[MARK] Matlab execute failed.\n");
	//}
	//if (FileIo_IsFileExist(_T("D:\\Result\\LOTID_202108051224"), _T("runout.txt")) == TRUE)
	//{
	//	DBGPRINT("[MARK] 있음.\n");
	//}
	//else
	//{
	//	DBGPRINT("[MARK] 없음.\n");
	//}
	DBGPRINT("끝\n");
}

MATRIX CTestApp2Dlg::Mat_Load_s(char* pszName)
{
	int32		i, j;
	MATRIX		M;
	mcxt				watch;
	double64			elapse;

	watch = Time_STAlloc();
	Time_STTick(watch);
	std::string line, str2;
	std::string word, temp;

	int Line_number = 0;
	int Value_number = 0;
	float temp_num = 0.0;

	std::vector<std::string> row;				// 1D vector
	std::vector<std::vector<std::string>> vect;	// 2D vector
	std::ifstream myfile(pszName);

	Time_STTock(watch, &elapse);
	DBGPRINT("hr_tm1  = %f\n", elapse);
	if (myfile.is_open())
	{
		Line_number = 0;
		while (getline(myfile, line))
		{
			++Line_number;
			str2 = line;

			std::stringstream s(str2);

			Value_number = 0;
			row.clear();

			// read every column data of a row and
			// store it in a string variable, 'word'
			while (getline(s, word, ',')) {
				++Value_number;
				// add all the column data
				// of a row to a vector
				row.push_back(word);
			}
			// Pushing back above 1D vector (row)
			// to create the 2D vector (vect)
			vect.push_back(row);
		}
		myfile.close();

		Time_STTock(watch, &elapse);
		DBGPRINT("hr_tm2  = %f\n", elapse);
		M = MAT_ALLOC(Line_number, Value_number);
		// CSV file loading and put into 2D array (vect) test
		for (i = 0; i < Line_number; ++i)
		{
			for (j = 0; j < Value_number; ++j)
			{
				MAT(M, i, j) = stof(vect[i][j]);
				//DBGPRINT("temp_num = %f\n", temp_num);
			}
		}
		Time_STTock(watch, &elapse);
		DBGPRINT("hr_tm3  = %f\n", elapse);
		Time_STFree(watch);
		return M;
	}
	return FALSE;
}

double CTestApp2Dlg::GetFocusValue(Img2D o)
{
	Img2D oR, oG, oB, oE;
	mcxt		watch;
	double64	elapse;
	int32       cnt = 0;
	ROI_s			roi;
	double dSum = 0.f;

	int focuse = 1;
	DBGPRINT("1\n");
	watch = Time_STAlloc();
	DBGPRINT("2\n");
	Time_STTick(watch);
	dSum = 0.f;
	DBGPRINT("3, %d\n", o);
	im3chan(o, &oR, &oG, &oB);
	DBGPRINT("GetFocusValue_%d,%d\n", oG->iHeight, oG->iWidth);
	if (focuse == 1)  // og
	{
		for (int iY = 0; iY < oG->iHeight; iY += 8)
			for (int iX = 1; iX < oG->iWidth - 2; iX++)
			{
				int iAddr = iY * oG->iWidth + iX;
				unsigned char* p = (unsigned char*)oG->pData + iAddr;
				dSum += abs(*p - *(p + 1));
				cnt++;
			}
		oE = oG;
	}
	else if (focuse == 2)  // 중심 좌우 한줄 상하 한줄만 계산
	{
		oE = imedge(oG, "sobel", 190);

		int iY = oE->iHeight / 2;
		for (int iX = 0; iX < oE->iWidth; iX++)//좌우
		{
			int iAddr = iY * oE->iWidth + iX;
			unsigned char* p = (unsigned char*)oE->pData + iAddr;

			dSum += abs(*p - *(p + 1));
			cnt++;
		}

		int iX = oE->iWidth / 2;
		for (int iY = 0; iY < oE->iHeight; iY++) //상하
		{
			int iAddr = iY * oE->iWidth + iX;
			unsigned char* p = (unsigned char*)oE->pData + iAddr;

			dSum += abs(*p - *(p + oE->iWidth));
			cnt++;
		}
	}
	else // 마스크 중심부 100 * 100
	{
		roi.x1 = (oG->iWidth / 2) - 150;
		roi.y1 = (oG->iHeight / 2) - 150;
		roi.x2 = (oG->iWidth / 2) + 150;
		roi.y2 = (oG->iHeight / 2) + 150;

		oE = im2sub(oG, &roi);

		oE = imedge(oE, "sobel", 190);

		for (int iY = 0; iY < oE->iHeight; iY++)
			for (int iX = 0; iX < oE->iWidth; iX++)
			{
				int iAddr = iY * oE->iWidth + iX;
				unsigned char* p = (unsigned char*)oE->pData + iAddr;

				dSum += abs(*p - *(p + 1));
				cnt++;
			}

		//for (int iY = (oE->iHeight / 2) - 50; iY < (oE->iHeight / 2) + 50; iY++)
		//	for (int iX = (oE->iWidth / 2) - 50; iX < (oE->iWidth / 2) + 50; iX++)
		//	{
		//		int iAddr = iY * oE->iWidth + iX;
		//		unsigned char *p = (unsigned char *)oE->pData + iAddr;

		//		dSum += abs(*p - *(p + 1));
		//		cnt++;
		//	}
	}
	Time_STTock(watch, &elapse);

	DBGPRINT(("dSum = %f / Time = %f / cnt = %d\n"), dSum, elapse, cnt);

	cnt = 0;
	//char		str[128];

	//sprintf(str, "oE_%f.bmp", dSum);
	//imwrite(oE, str, "bmp");

	IMP_FREE(oR);
	IMP_FREE(oG);
	IMP_FREE(oB);
	IMP_FREE(oE);

	Time_STFree(watch);
	return dSum;
}

char* CTestApp2Dlg::UIstring(char* str)
{
	static char		gUIstring[UI_STRING_NUM][UI_STRING_LEN + 1];
	static boolean		gUIstring_init = FALSE;
	int32			i;
	char* ptr;

	if (gUIstring_init == FALSE)
	{
		for (i = 0; i < UI_STRING_NUM; i++)
		{
			ptr = &gUIstring[i][0];
			memZeros(ptr, (UI_STRING_LEN + 1));
		}
		gUIstring_init = TRUE;
	}
	if (str != NULL)
	{
		*(str + UI_STRING_LEN) = 0x00;
		memZeros(str, UI_STRING_LEN);
		return	NULL;
	}

	for (i = 0; i < UI_STRING_NUM; i++)
	{
		ptr = &gUIstring[i][0];
		if ((*(ptr + UI_STRING_LEN)) == 0x00)
		{
			*(ptr + UI_STRING_LEN) = 0x01;
			return	ptr;
		}
	}

	return	NULL;
}

//void* CTestApp2Dlg::memCalloc_(dword siz, const char* pszFile, int32 nLine)
//{
//	void* ptr;
//	byte		j;
//
//	for (j = 0; j < AUXMEM_TBL_MAX; j++)
//	{
//		if (memTbl[j] == NULL)
//		{
//			memTbl[j] = memTbl_Create(j, AUXMEM_LIST_SIZE, auxMem_CBF);
//		}
//		ptr = memGet(memTbl[j], siz, 0, pszFile, nLine);
//		if (ptr != NULL)
//		{
//			memZeros(ptr, siz);
//			break;
//		}
//	}
//	if (ptr == NULL)
//	{
//		if (pszFile != NULL)
//		{
//			LOGWARN(_T("[auxMem] Out of memory block(%s,%d)"), pszFile, nLine);
//		}
//		else
//		{
//			LOGWARN(_T("[auxMem] Out of memory block"));
//		}
//	}
//	return	ptr;
//}

void CTestApp2Dlg::memFree_(void* ptr, const char* pszFile, int32 nLine)
{
	MEMHEADER_s* h;

	if (memAssert(ptr) == FALSE)
	{
		if (ptr == NULL)
			return;

		if (pszFile != NULL)
		{
			LOGWARN(_T("[auxMem] Try to free the invalid memory(%s,%d)"), pszFile, nLine);
		}
		else
		{
			LOGWARN(_T("[auxMem] Try to free the invalid memory"));
		}
		return;
	}
	h = (MEMHEADER_s*)((byte*)ptr - sizeof(*h));

	if (!RANGE_IN(h->tblID, 0, (AUXMEM_TBL_MAX - 1)))
	{
		if (pszFile != NULL)
		{
			LOGWARN(_T("[auxMem] Try to free the invalid memory(%s,%d)"), pszFile, nLine);
		}
		else
		{
			LOGWARN(_T("[auxMem] Try to free the invalid memory"));
		}
		return;
	}
	memPut(memTbl[h->tblID], ptr, pszFile, nLine);
}

LRESULT CTestApp2Dlg::auxMem_CBF(dword cmd, WPARAM wp, LPARAM lp, void* ptr)
{
	switch (cmd)
	{
	case MEM_ALLOC_C:
	{
		return	(LRESULT)osCalloc(wp);
	}
	case MEM_REALLOC_C:
	{
		if (ptr != NULL)
		{
			osFree(ptr);
		}
		return	(LRESULT)osCalloc(wp);
	}
	case MEM_FREE_C:
	{
		if (ptr != NULL)
		{
			osFree(ptr);
		}
		break;
	}
	case MEM_ERROR_C:
	{
		LOGWARN(_T("[auxMem] Memory internal handling error"));
		break;
	}
	}

	return	(0);
}

Img2D CTestApp2Dlg::cv_imread(const TCHAR* pszname, int32 w, int32 h)
{
	Img2D		a = NULL, c0, c1, c2;
	int			i, j;
	int32		index;
	byte* s;

	CStringA pszname_c(pszname);

	if (pszname == NULL)
		return FALSE;

	cv::Mat a_m = cv::imread(pszname_c.GetBuffer(0));
	DBGPRINT("hr a_m.type = %d, a_m.cols = %d, a_m.rows = %d, a_m.data = %d\n\n", a_m.type(), a_m.cols, a_m.rows, a_m.data);

	if (a_m.empty())
		return FALSE;

	if (w > 1 && h > 1)
	{
		cv::resize(a_m, a_m, cv::Size(w, h));
	}

	if (a_m.empty())
		return FALSE;

	c0 = IMP_ALLOC(IMG_TYPE_BIT8, a_m.cols, a_m.rows, TRUE);
	c1 = IMP_ALLOC(IMG_TYPE_BIT8, a_m.cols, a_m.rows, TRUE);
	c2 = IMP_ALLOC(IMG_TYPE_BIT8, a_m.cols, a_m.rows, TRUE);


	for (j = 0; j < a_m.rows; j++)
	{
		for (i = 0; i < a_m.cols; i++)
		{
			index = (j * a_m.cols + i) * 3;
			s = (byte*)c0->pData + i + (j * c0->iWidth);
			*s = a_m.data[index + 2];
			s = (byte*)c1->pData + i + (j * c1->iWidth);
			*s = a_m.data[index + 1];
			s = (byte*)c2->pData + i + (j * c2->iWidth);
			*s = a_m.data[index];
		}
	}
	a = im3chan_merge(c0, c1, c2);

	IMP_FREE(c0);
	IMP_FREE(c1);
	IMP_FREE(c2);

	return a;
}

boolean CTestApp2Dlg::cv_imwrite(Img2D o, const char* pszname, int32 w, int32 h)
{
	Img2D		a = NULL, c0, c1, c2;
	int			i, j;
	int32		index;
	byte* s;
	char		str[128];

	if (o == NULL)
		return FALSE;

	if (pszname == NULL)
		return FALSE;

	cv::Mat a_m(o->iHeight, o->iWidth, CV_8UC3);
	DBGPRINT("hr w a_m.type = %d, a_m.cols = %d, a_m.rows = %d, a_m.data = %d\n\n", a_m.type(), a_m.cols, a_m.rows, a_m.data);

	c0 = IMP_ALLOC(IMG_TYPE_BIT8, o->iWidth, o->iHeight, TRUE);
	c1 = IMP_ALLOC(IMG_TYPE_BIT8, o->iWidth, o->iHeight, TRUE);
	c2 = IMP_ALLOC(IMG_TYPE_BIT8, o->iWidth, o->iHeight, TRUE);

	im3chan(o, &c0, &c1, &c2);

	for (j = 0; j < a_m.rows; j++)
	{
		for (i = 0; i < a_m.cols; i++)
		{
			index = (j * a_m.cols + i) * 3;
			s = (byte*)c0->pData + i + (j * c0->iWidth);
			a_m.data[index + 2] = *s;
			s = (byte*)c1->pData + i + (j * c1->iWidth);
			a_m.data[index + 1] = *s;
			s = (byte*)c2->pData + i + (j * c2->iWidth);
			a_m.data[index] = *s;
		}
	}
	a = im3chan_merge(c0, c1, c2);
	DBGPRINT("hr a->byType = %d, a->iWidth = %d, a->iHeight = %d, a->pData = %d\n\n", a->byType, a->iWidth, a->iHeight, a->pData);

	if (w > 1 && h > 1)
	{
		cv::resize(a_m, a_m, cv::Size(w, h));
	}

	sprintf(str, "hr_jpg3.bmp");
	imwrite(a, str, "bmp");

	cv::imwrite(pszname, a_m);

	IMP_FREE(a);
	IMP_FREE(c0);
	IMP_FREE(c1);
	IMP_FREE(c2);

	return TRUE;
}

void CTestApp2Dlg::cv_imread_test1()
{
	char		str[128];
	//Img2D o = imread("C:\\TestApp2\\bin\\224.805_114.903.jpg", "jpg");

	Img2D		o = imread("C:\\TestApp2\\bin\\TEST_3.bmp", "bmp");
	if (o)
	{
		sprintf(str, "hr_1.bmp");
		imwrite(o, str, "bmp");
		DBGPRINT("hr o = %s\n", str);
		DBGPRINT("hr o->byType = %d, o->iWidth = %d, o->iHeight = %d, o->pData = %d\n\n", o->byType, o->iWidth, o->iHeight, o->pData);
	}

	//cv::Mat o2 = cv::imread("C:\\TestApp2\\bin\\224.805_114.903.jpg");
	cv::Mat o2 = cv::imread("C:\\TestApp2\\bin\\TEST_3.bmp");
	if (!o2.empty())
	{
		sprintf(str, "hr_2.bmp");
		cv::imwrite(str, o2);
		DBGPRINT("hr o2 = %s\n", str);
	}

	DBGPRINT("hr o2.type = %d, o2.cols = %d, o2.rows = %d, o2.data = %d\n\n", o2.type(), o2.cols, o2.rows, o2.data);

	Img2D o3 = cv_imread(_T("C:\\TestApp2\\bin\\TEST_3.bmp"), false, false);
	sprintf(str, "hr_3.bmp");
	imwrite(o3, str, "bmp");
	DBGPRINT("hr o3 = %s\n", str);
	DBGPRINT("hr o3->byType = %d, o3->iWidth = %d, o3->iHeight = %d, o3->pData = %d\n\n", o3->byType, o3->iWidth, o3->iHeight, o3->pData);


	IMP_FREE(o);
	IMP_FREE(o3);
}

void CTestApp2Dlg::cv_imwrite_test1()
{
	char		str[128];
	//Img2D o = imread("C:\\TestApp2\\bin\\224.805_114.903.jpg", "jpg");

	cv::Mat o = cv::imread("C:\\TestApp2\\bin\\이지vs조던.jpg");
	if (!o.empty())
	{
		sprintf(str, "hr_jpg1.jpg");
		cv::imwrite(str, o);
		DBGPRINT("hr o = %s\n", str);
		DBGPRINT("hr o.type = %d, o2.cols = %d, o.rows = %d, o.data = %d\n\n", o.type(), o.cols, o.rows, o.data);
	}

	Img2D o2 = cv_imread(_T("C:\\TestApp2\\bin\\이지vs조던.jpg"), false, false);
	if (o2)
	{
		sprintf(str, "hr_jpg2.jpg");
		cv_imwrite(o2, str, false, false);
		DBGPRINT("hr o2 = %s\n", str);
		DBGPRINT("hr o2->byType = %d, o2->iWidth = %d, o2->iHeight = %d, o2->pData = %d\n\n", o2->byType, o2->iWidth, o2->iHeight, o2->pData);
	}

	IMP_FREE(o2);
}

void CTestApp2Dlg::Img2D_FREE_TEST()
{
	char		str[128];

	Img2D o2 = cv_imread(_T("C:\\TestApp2\\bin\\이지vs조던.jpg"), false, false);

	sprintf(str, "hr_imp_free_jpg1.jpg");
	cv_imwrite(o2, str, false, false);

	IMP_FREE(o2);

	sprintf(str, "hr_imp_free_jpg2.jpg");
	cv_imwrite(o2, str, false, false);
}

void CTestApp2Dlg::ntg_filter_test()
{
	MATRIX raw_data;
	int num_data = 0;
	TCHAR str[128];
	VECTOR NTG_DATA_X;
	VECTOR NTG_DATA_T;

	DBGPRINT("hr1 테스트 데이터 생성\n");
	NTG_DATA_X = Vec_Load("C:\\TestApp2\\bin\\NTG_TEST\\CHECK_DATA\\NTG_DATA_X.txt");
	NTG_DATA_T = Vec_Load("C:\\TestApp2\\bin\\NTG_TEST\\CHECK_DATA\\NTG_DATA_T.txt");

	DBGPRINT("NTG_DATA_X->iNum = % d\n", NTG_DATA_X->iNum);
	raw_data = MAT_ALLOC(2, NTG_DATA_X->iNum);
	for (int32 i = 0; i < NTG_DATA_X->iNum; i++)
	{
		MAT(raw_data, 0, i) = VEC(NTG_DATA_X, i);
		MAT(raw_data, 1, i) = VEC(NTG_DATA_T, i);
	}

	MATRIX raw_data_ip = Mat_GetChild(raw_data, 0, 1, 0, raw_data->iY-1);
	DBGPRINT("raw_data->iY = % d\n", raw_data->iY);
	DBGPRINT("hr2 데이터 필터링_raw_data_ip\n");

	MATRIX thk_filt_op = MAT_ALLOC(raw_data->iX, raw_data->iY);
	thk_filt(raw_data_ip, thk_filt_op, num_data);
	DBGPRINT("hr2 데이터 필터링_thk_filt_op\n");

	//MATRIX std_filt_op = MAT_ALLOC(raw_data_ip->iX, raw_data_ip->iY);
	//med_std_filt(raw_data_ip, std_filt_op, num_data);
	//DBGPRINT("hr2 데이터 필터링_med_std_filt\n");

	MATRIX std_filt_ip = Mat_GetChild(thk_filt_op, 0, 1, 0, num_data - 1);
	MATRIX std_filt_op = MAT_ALLOC(thk_filt_op->iX, thk_filt_op->iY);
	med_std_filt(std_filt_ip, std_filt_op, num_data);
	DBGPRINT("hr2 데이터 필터링_med_std_filt\n");

	MATRIX int_filt_ip = Mat_GetChild(std_filt_op, 0, 1, 0, num_data - 1);
	int Outsize = (int)(MAT(int_filt_ip, 0, int_filt_ip->iY - 1)) - (int)(MAT(int_filt_ip, 0, 0));
	DBGPRINT("Outsize = %d\n", Outsize);
	//MATRIX	int_filt_op = MAT_ALLOC(int_filt_ip->iX, Outsize - 1);
	//lin_interpol(int_filt_ip, int_filt_op);
	//DBGPRINT("hr2 데이터 필터링_lin_interpol\n"); //1mm데이터변경

	MATRIX mov_ave_op = MAT_ALLOC(int_filt_ip->iX, int_filt_ip->iY);
	mov_aver_filt(int_filt_ip, mov_ave_op);
	DBGPRINT("hr2 데이터 필터링_mov_ave_op\n");
	MatrixSaveTimeStampC(mov_ave_op, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr6_mov_ave_op.txt");

	//int data_count = 0;
	//int step_size = 5;
	//int tmp_data = 0;
	//int tmp_int = 0;

	//for (int index = 0; index < mov_ave_op->iY; index++)
	//{
	//	tmp_data = (int)MAT(mov_ave_op, 0, index);
	//	tmp_int = ((int)(tmp_data));
	//	if (0 == (tmp_int % step_size))
	//	{
	//		MAT(mov_ave_op, 0, data_count) = MAT(mov_ave_op, 0, index);
	//		MAT(mov_ave_op, 1, data_count) = MAT(mov_ave_op, 1, index);
	//		data_count++;
	//	}
	//}
	//MATRIX ret_filt_data = Mat_GetChild(mov_ave_op, 0, 1, 0, data_count - 1);
	//DBGPRINT("hr2 데이터 필터링_5mm\n");

	DBGPRINT("hr3 데이터 저장\n");

	Mat_SetSaveDigit(FALSE, 4);
	MatrixSaveTimeStampC(raw_data_ip, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr1_raw_data.txt");
	MatrixSaveTimeStampC(thk_filt_op, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr2_thk_filt_op.txt");
	MatrixSaveTimeStampC(std_filt_op, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr3_std_filt_op.txt");
	MatrixSaveTimeStampC(int_filt_ip, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr4_int_filt_ip.txt");
	//MatrixSaveTimeStampC(int_filt_op, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr5_int_filt_op.txt");
	//MatrixSaveTimeStampC(ret_filt_data, "C:\\TestApp2\\bin\\NTG_TEST\\FILTER_DATA\\", "hr7_ret_filt_data.txt");

	_stprintf/*sprintf*/(str, _T("C:\\TestApp2\\bin\\NTG_TEST\\VISION_DATA\\vision.csv"));

	mcxt vision = FileIo_Open(str, FIO_OPEN_WR);
	
#if 1
	//filter
	VECTOR x = Mat_GetColumn(mov_ave_op, 0);
	VECTOR yfft = Mat_GetColumn(mov_ave_op, 1);

#else
	//raw
	VECTOR x = Mat_GetColumn(raw_data, 0);
	VECTOR yfft = Mat_GetColumn(raw_data, 1);
#endif

	FileIo_Printf(vision, "Tank,%s\r\n","KC06");
	FileIo_Printf(vision, "BODID,%s\r\n", "KC06");
	FileIo_Printf(vision, "Measurement Date,%00004d-%002d-%002d \r\n", 2012, 02, 18);
	FileIo_Printf(vision, "Measurement Time,%002d:%002d:%002d \r\n", 17, 10, 24);
	FileIo_Printf(vision, "Alarm,%d \r\n", 0);
	FileIo_Printf(vision, "Purpose,Production \r\n");
	FileIo_Printf(vision, "Product Code,G7(1915) \r\n");
	FileIo_Printf(vision, "Glass Code,E-XG \r\n");
	FileIo_Printf(vision, "Glass Type,G \r\n");
	FileIo_Printf(vision, "Glass RefIdx,%.4f \r\n", 1.516);
	FileIo_Printf(vision, "Product Width (mm),%.1f \r\n", 1780);
	FileIo_Printf(vision, "Product Height (mm),%d \r\n", 2200);
	FileIo_Printf(vision, "Product Thickness (mm),%.1f \r\n", 0.25);
	FileIo_Printf(vision, "Trim Inlet (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "Trim Compression (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "VBS Offset (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "Full Sheet Width (mm),%.0f\r\n", 1);//FIX
	FileIo_Printf(vision, "QA Sheet Width (mm),%.1f\r\n", 1);//FIX
	FileIo_Printf(vision, "Leading Edge,Compression \r\n");
	FileIo_Printf(vision, "Calibration Standard,D25 \r\n");
	FileIo_Printf(vision, "Calibration Glass Code,%d \r\n", 2000);
	FileIo_Printf(vision, "Calibration Glass Type,G \r\n");
	FileIo_Printf(vision, "Calibration Glass RefIdx,%.4f \r\n", 1.516);
	FileIo_Printf(vision, "Encoder Count,%d \r\n", 0);
	FileIo_Printf(vision, "Encoder mm per Count,%.6f \r\n", 0.0);
	FileIo_Printf(vision, "Cycle Time,%d \r\n", 11);
	FileIo_Printf(vision, "Number of Bad Points,%d \r\n", 0);
	FileIo_Printf(vision, "Number of Reported Points,%d \r\n", 4000);
	FileIo_Printf(vision, "Position,Thickness\r\n");

	for (int32 k = 0; k < yfft->iNum -1; k++)
	{
		if (VEC(yfft,k) != 0)
		{
			FileIo_Printf(vision, "%.1f,%.4f\r\n", VEC(x, k), VEC(yfft, k));
		}
	}

	DBGPRINT("전송용 DATA 저장완료 %d", yfft->iNum -1);
	FileIo_Close(vision);

	MAT_FREE(raw_data);
	MAT_FREE(raw_data_ip);
	MAT_FREE(thk_filt_op);
	MAT_FREE(std_filt_ip);
	MAT_FREE(std_filt_op);
	MAT_FREE(int_filt_ip);
	//MAT_FREE(int_filt_op);
	MAT_FREE(mov_ave_op);
	//MAT_FREE(ret_filt_data);
}

void CTestApp2Dlg::ntg_filter_test2()
{
	VECTOR TC_x;
	VECTOR TC_yfft;
	VECTOR NTG_DATA_X;
	VECTOR NTG_DATA_T;
	TCHAR str[128];

	DBGPRINT("hr1 테스트 데이터 생성\n");
	NTG_DATA_X = Vec_Load("D:\\TestApp3\\NTG_TEST\\CHECK_DATA\\NTG_DATA_X.txt");
	NTG_DATA_T = Vec_Load("D:\\TestApp3\\NTG_TEST\\CHECK_DATA\\NTG_DATA_T.txt");

	ntg_filter_test_TC17(NTG_DATA_X, NTG_DATA_T, TC_x, TC_yfft);

	DBGPRINT("전송용 DATA 저장시작 %d\n", TC_yfft->iNum - 1);
	_stprintf/*sprintf*/(str, _T("D:\\TestApp3\\NTG_TEST\\VISION_DATA\\vision.csv"));
	mcxt vision = FileIo_Open(str, FIO_OPEN_WR);

	FileIo_Printf(vision, "Tank,%s\r\n", "KC06");
	FileIo_Printf(vision, "BODID,%s\r\n", "KC06");
	FileIo_Printf(vision, "Measurement Date,%00004d-%002d-%002d \r\n", 2012, 02, 18);
	FileIo_Printf(vision, "Measurement Time,%002d:%002d:%002d \r\n", 17, 10, 24);
	FileIo_Printf(vision, "Alarm,%d \r\n", 0);
	FileIo_Printf(vision, "Purpose,Production \r\n");
	FileIo_Printf(vision, "Product Code,G7(1915) \r\n");
	FileIo_Printf(vision, "Glass Code,E-XG \r\n");
	FileIo_Printf(vision, "Glass Type,G \r\n");
	FileIo_Printf(vision, "Glass RefIdx,%.4f \r\n", 1.516);
	FileIo_Printf(vision, "Product Width (mm),%.1f \r\n", 1780);
	FileIo_Printf(vision, "Product Height (mm),%d \r\n", 2200);
	FileIo_Printf(vision, "Product Thickness (mm),%.1f \r\n", 0.25);
	FileIo_Printf(vision, "Trim Inlet (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "Trim Compression (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "VBS Offset (mm),%.1f\r\n", 1);
	FileIo_Printf(vision, "Full Sheet Width (mm),%.0f\r\n", 1);//FIX
	FileIo_Printf(vision, "QA Sheet Width (mm),%.1f\r\n", 1);//FIX
	FileIo_Printf(vision, "Leading Edge,Compression \r\n");
	FileIo_Printf(vision, "Calibration Standard,D25 \r\n");
	FileIo_Printf(vision, "Calibration Glass Code,%d \r\n", 2000);
	FileIo_Printf(vision, "Calibration Glass Type,G \r\n");
	FileIo_Printf(vision, "Calibration Glass RefIdx,%.4f \r\n", 1.516);
	FileIo_Printf(vision, "Encoder Count,%d \r\n", 0);
	FileIo_Printf(vision, "Encoder mm per Count,%.6f \r\n", 0.0);
	FileIo_Printf(vision, "Cycle Time,%d \r\n", 11);
	FileIo_Printf(vision, "Number of Bad Points,%d \r\n", 0);
	FileIo_Printf(vision, "Number of Reported Points,%d \r\n", 4000);
	FileIo_Printf(vision, "Position,Thickness\r\n");

	for (int32 k = 0; k < TC_yfft->iNum - 1; k++)
	{
		if (VEC(TC_yfft, k) != 0)
		{
			FileIo_Printf(vision, "%.1f,%.4f\r\n", VEC(TC_x, k), VEC(TC_yfft, k));
		}
	}

	DBGPRINT("전송용 DATA 저장완료 %d\n", TC_yfft->iNum - 1);
	FileIo_Close(vision);

}