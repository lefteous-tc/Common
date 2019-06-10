#pragma once

#include "CustomStringList.h"

class StringList : public CustomStringList
{
public:
	StringList ():
	m_pCurrentElem(NULL)
	{		
	}
//-----------------------------------------------------------
	void push (const wchar_t* array)
	{
		insert (m_pEnd, array);
	}
//-----------------------------------------------------------
	wchar_t* getFirst ()
	{
		m_pCurrentElem = m_pBegin;
		if (m_pCurrentElem/* && m_pCurrentElem != m_pEnd*/)
		{
			return m_pCurrentElem->m_Value; 
		}
		return NULL;
	}
//-----------------------------------------------------------
	wchar_t* getNext ()
	{
		if (!m_pCurrentElem && m_pCurrentElem == m_pEnd)
		{
			return NULL;
		}

		m_pCurrentElem = m_pCurrentElem->m_pNext;
		if (m_pCurrentElem)
		{
			return m_pCurrentElem->m_Value;			
		}
		return NULL;
	}
//-----------------------------------------------------------
	wchar_t* removeCurrent ()
	{
		ListElem* pCurrentElem = NULL;
		if (m_pCurrentElem != m_pEnd)
		{
			pCurrentElem = m_pCurrentElem->m_pNext;
		}
		remove (m_pCurrentElem, NULL);
		m_pCurrentElem = pCurrentElem;
		if (m_pCurrentElem)
		{
			return m_pCurrentElem->m_Value;
		}
		return NULL;
	}
//-----------------------------------------------------------
	BOOL hasElements ()
	{
		return m_pBegin != NULL;
	}
//-----------------------------------------------------------
	unsigned long getNumberOfElements ()
	{
		wchar_t* c = getFirst();
		unsigned long elements = 0;
		while (c)
		{
			elements++;
			c = getNext();
		}
		return elements;
	}
	//-----------------------------------------------------------
	BOOL enumStarted ()
	{
		return m_pCurrentElem != NULL;
	}
	//-----------------------------------------------------------
	void clear ()
	{		
		while (m_pBegin)
		{
			remove (m_pBegin, NULL);
		}
	}
	//-----------------------------------------------------------
	BOOL isInList (const wchar_t* string)
	{
		BOOL inList = FALSE;
		if (!string)
		{
			return inList;
		}
		wchar_t* c = getFirst();
		while (c && !inList)
		{
			if (_wcsicmp (c, string) == 0)
			{ 
				inList = TRUE;
			}
			c = getNext();
		}
		return inList;
	}
	//-----------------------------------------------------------
protected:
	ListElem* m_pCurrentElem;

};