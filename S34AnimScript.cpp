// S34AnimScript.cpp: implementation of the CS34AnimScript class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "S34AnimScript.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS34AnimScript::CS34AnimScript()
{

}

CS34AnimScript::~CS34AnimScript()
{
	Clear();
}

int CS34AnimScript::AddCommand(int iCommandID, int iValue)
{
	DWORD *pInstruction = new DWORD;
	if (pInstruction==NULL)
		return S34ANM_E_MEMORYALLOCATION_FAILED;

	*pInstruction = MAKELONG(iValue,iCommandID);
	m_InstructionQueue.Add(pInstruction);
	return S34ANM_E_NOERROR;
}

int CS34AnimScript::GetCommand(int &iCommandID, int &iValue)
{
	DWORD *pData = (DWORD*)m_InstructionQueue.GetData();
	if (pData == NULL)
	{
		iCommandID = S34ANIM_END;
		iValue = 0;
		return S34ANM_E_SCRIPT_INSTRUCTION_QUEUE_EMPTY;
	}
	iCommandID = HIWORD(*pData);
	iValue = LOWORD(*pData);
	return S34ANM_E_NOERROR;
}

int CS34AnimScript::Clear()
{
	DWORD *pInstruction;
	if (m_InstructionQueue.MoveToHead())
	{			
		do 
		{
			pInstruction = (DWORD*)m_InstructionQueue.GetData();
			delete pInstruction;
			pInstruction = NULL;
		} while (m_InstructionQueue.MoveNext());
	}
	m_InstructionQueue.Clear();
	return S34ANM_E_NOERROR;
}


int CS34AnimScript::GotoStart()
{
	if (!m_InstructionQueue.MoveToHead())
		return S34ANM_E_SCRIPT_INSTRUCTION_QUEUE_EMPTY;
	return S34ANM_E_NOERROR;
}

int CS34AnimScript::GotoNext()
{
	if (!m_InstructionQueue.MoveNext())
		return S34ANM_E_SCRIPT_END_OF_QUEUE;
	return S34ANM_E_NOERROR;
}


int CS34AnimScript::LoadAnimScript(char *pFileName)
{
	// clear current script, if any
	//
	Clear();

	// Load script
	//
	HANDLE hFile;
	hFile = ::CreateFile((const char*)pFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return S34ANM_E_FILEOPEN_FAILED;
	}

	DWORD dwRead;
	DWORD dwSize = GetFileSize(hFile, NULL);
	int iInstructionCount = dwSize>>2;
	DWORD *pCommands = new DWORD[iInstructionCount];
	if (pCommands==NULL)
	{
		return S34ANM_E_MEMORYALLOCATION_FAILED;
	}

	if (!::ReadFile((HANDLE)hFile, pCommands, dwSize, &dwRead, NULL))
	{
		delete []pCommands;
		CloseHandle((HANDLE)hFile);
		return S34ANM_E_FILEREAD_FAILED;
	}

	int iCommandID, iValue;
	for (int i=0; i < iInstructionCount; i++)
	{
		iCommandID = LOWORD(pCommands[i]);
		iValue = HIWORD(pCommands[i]);
		AddCommand(iCommandID, iValue);
	}

	delete []pCommands;
	CloseHandle((HANDLE)hFile);

	return S34ANM_E_NOERROR;
}

