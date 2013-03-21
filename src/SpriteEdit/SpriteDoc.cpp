// SpriteDoc.cpp : implementation of the CSpriteDoc class
//

#include "stdafx.h"
#include "SpriteEdit.h"
#include "SpriteView.h"
#include "RootDlg.h"
#include <shlwapi.h>
#include <direct.h>



#include "SpriteDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#pragma comment(lib, "Shlwapi.lib")

#define PROJECT_ROOT_STRING "; $EDITOR_PROJECT_ROOT_DIR$ "



/////////////////////////////////////////////////////////////////////////////
// CSpriteDoc

IMPLEMENT_DYNCREATE(CSpriteDoc, CDocument)

BEGIN_MESSAGE_MAP(CSpriteDoc, CDocument)
	//{{AFX_MSG_MAP(CSpriteDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpriteDoc construction/destruction

//////////////////////////////////////////////////////////////////////////
CSpriteDoc::CSpriteDoc()
{
	// TODO: add one-time construction code here

}


//////////////////////////////////////////////////////////////////////////
CSpriteDoc::~CSpriteDoc()
{
}


//////////////////////////////////////////////////////////////////////////
BOOL CSpriteDoc::OnNewDocument()
{
	if(!QueryProjectRoot()) return FALSE;	

	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSpriteDoc serialization

void CSpriteDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSpriteDoc diagnostics

#ifdef _DEBUG
void CSpriteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSpriteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSpriteDoc commands


//////////////////////////////////////////////////////////////////////////
BOOL CSpriteDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CStdioFile f;
	CString line;
	bool RootFound = false;
	if(f.Open(lpszPathName, CFile::modeRead | CFile::typeText)){
		while(f.ReadString(line)){
			if(line.Find(PROJECT_ROOT_STRING)==0){
				CString root = line.Right(line.GetLength() - strlen(PROJECT_ROOT_STRING));
				root.TrimLeft();
				root.TrimRight();


				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				_splitpath(lpszPathName, drive, dir, NULL, NULL);
				
				char AbsPath[MAX_PATH];
				sprintf(AbsPath, "%s%s", drive, dir);
				chdir(AbsPath);
				_fullpath(AbsPath, LPCSTR(root), MAX_PATH);

				m_ProjectRoot = CString(AbsPath);
				if(m_ProjectRoot[m_ProjectRoot.GetLength()-1]!='\\') m_ProjectRoot+="\\";

				DWORD attr = GetFileAttributes(LPCSTR(AbsPath));
				if(attr == 0xFFFFFFFF || (attr & FILE_ATTRIBUTE_DIRECTORY) == 0){
					RootFound = false;
				}
				else{					
					RootFound = true;
				}

				break;
			}
		}
		f.Close();
	}

	if(RootFound){
		((CSpriteEditApp*)AfxGetApp())->AddProjectRoot(m_ProjectRoot);
		return TRUE;
	}
	else return QueryProjectRoot(lpszPathName);
}


//////////////////////////////////////////////////////////////////////////
BOOL CSpriteDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	SetCurrentDirectory(m_ProjectRoot);

	POSITION pos = GetFirstViewPosition();
	CSpriteView* View = (CSpriteView*)GetNextView(pos);
	
	if(View){
		View->UpdateSelectionState();
		CBDynBuffer* buf = ::new CBDynBuffer(View->Game);
		buf->PutText("; generated by SpriteEdit\n\n");
		WriteEditorData(lpszPathName, buf);
		View->m_Sprite->SaveAsText(buf, 0);

		FILE* f = fopen(lpszPathName, "wt");
		if(!f){
			delete buf;
			return FALSE;
		}
		
		fwrite(buf->m_Buffer, buf->GetSize(), 1, f);
		delete buf;
		fclose(f);
		SetModifiedFlag(FALSE);
		return TRUE;
	}
	else return FALSE;

	//return CDocument::OnSaveDocument(lpszPathName);
}


//////////////////////////////////////////////////////////////////////////
bool CSpriteDoc::QueryProjectRoot(CString Filename)
{
	CFrameWnd* frame = ((CMDIFrameWnd*)AfxGetMainWnd())->GetActiveFrame();
		
	CRootDlg dlg;
	CSpriteEditApp* app = (CSpriteEditApp*)AfxGetApp();
	for(int i=0; i<app->m_ProjectRoots.GetSize(); i++){
		dlg.AddPrevFolder(app->m_ProjectRoots[i]);
	}
	
	dlg.m_Filename = Filename;
	dlg.m_SelectedFolder = m_ProjectRoot;
	
	if(dlg.DoModal()==IDOK){
		//MessageBox(NULL, dlg.m_SelectedFolder, "", MB_OK);
		if(frame) frame->ShowWindow(SW_SHOW);
		m_ProjectRoot = dlg.m_SelectedFolder;
		app->AddProjectRoot(dlg.m_SelectedFolder);
		return true;
	}
	else return false;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CSpriteDoc::WriteEditorData(LPCTSTR lpszPathName, CBDynBuffer *Buffer)
{
	char RelPath[MAX_PATH];
	//PathRelativePathTo(RelPath, LPCSTR(m_ProjectRoot), FILE_ATTRIBUTE_NORMAL, lpszPathName, FILE_ATTRIBUTE_NORMAL);
	if(!PathRelativePathTo(RelPath, lpszPathName, FILE_ATTRIBUTE_NORMAL, LPCSTR(m_ProjectRoot), FILE_ATTRIBUTE_NORMAL)){
		strcpy(RelPath, LPCSTR(m_ProjectRoot));
	}

	Buffer->PutTextIndent(0, "%s%s\n\n", PROJECT_ROOT_STRING, RelPath);
	return S_OK;
}
