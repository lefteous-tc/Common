//------------------------------------------------------------------------------
#ifndef CustomStringListH
#define CustomStringListH

#include <windows.h>
#include <strsafe.h>

//------------------------------------------------------------------------------
class CustomStringList
{
protected:
	class ListElem
    {
	public:
		wchar_t m_Value[MAX_PATH];
		ListElem* m_pNext;
		ListElem* m_pPrev;
//------------------------------------------------------------------------------
        // Konstruktor tr�gt value ein
		ListElem (const wchar_t* value):
        m_pNext(NULL), m_pPrev(NULL)
        {
			StringCchCopyW (m_Value, MAX_PATH, value);
        };
//------------------------------------------------------------------------------
	};
//------------------------------------------------------------------------------
	ListElem* m_pBegin;
	ListElem* m_pEnd;
//------------------------------------------------------------------------------
    // Konstruktor erzeugt leere Liste
	CustomStringList ():
    m_pBegin(NULL), m_pEnd(NULL)
    {
    };
//------------------------------------------------------------------------------
	// Destruktor l�scht auch CListElems
	virtual ~CustomStringList ()
    {
			while (m_pBegin!=NULL)
            {
				ListElem* pTemp = m_pBegin;
				m_pBegin = m_pBegin->m_pNext;
				delete [] pTemp;
			}
    };
//------------------------------------------------------------------------------
	CustomStringList (const CustomStringList& l);					// Copy-Konstruktor
	CustomStringList& operator= (const CustomStringList& l);	// Zuweisung
	// Copy-Konstruktor und Operator =
	// werden bewu�t nicht definiert; dies provoziert einen
	// Linker-Fehler bei versehentlichem Gebrauch
//------------------------------------------------------------------------------
	void insert (ListElem* pPos, const wchar_t* array)
	// Legt ein neues CListElem an und tr�gt value als Nutzinhalt
	// ein. Das CListElem wird HINTER dem Element, auf das pPos
	// zeigt, eingef�gt.
	// Sonderfall pPos==NULL: einf�gen am Anfang der Liste.
	// PRE:  gen�gend freier Speicher ist verf�gbar
	// POST: Liste ist um ein Element gewachsen
	{
		// neues Element allokieren:
		ListElem* pNew = new ListElem (array);		
		if (!pNew)
		  return;

		// einf�gen in die Liste:
		if (!m_pBegin) 
		{			// Sonderfall leere Liste
			m_pBegin = pNew;
			m_pEnd = pNew;
		}
		else if (!pPos) {		// es existiert ein Nachfolger
			pNew->m_pNext = m_pBegin;			// A: Verweis auf Nachfolger
			pNew->m_pNext->m_pPrev = pNew;	// B: Verweis von Nachfolger
			m_pBegin = pNew;						// C: Zeiger auf erstes Element
		}
		else if (pPos == m_pEnd) {		// es existiert ein Vorg�nger
			pPos->m_pNext = pNew;				// A: Verweis von Vorg�nger
			pNew->m_pPrev = pPos;				// B: Verweis auf Vorg�nger
			m_pEnd = pNew;							// C: Zeiger auf letztes Element
		}
		else {								// Vorg�nger UND Nachfolger existieren
			pNew->m_pNext = pPos->m_pNext;	// B: Verweis auf Nachfolger
			pNew->m_pPrev = pPos;				// A: Verweis auf Vorg�nger
			pPos->m_pNext = pNew;				// A: Verweis von Vorg�nger
			pNew->m_pNext->m_pPrev = pNew;	// B: Verweis von Nachfolger
		}
	};
//------------------------------------------------------------------------------
	void remove (ListElem* pPos, wchar_t* removedValue)
	// L�scht das Listenelement Pos aus der Liste und liefert
	// seinen Nutzinhalt als Funktionsergebnis ab.
	// PRE:  Pos!=NULL && Pos ist Element der Liste
	// POST: Liste ist um ein Element geschrumpft
	{
		//if (!pPos)
		  //throw "Parameter of remove is NULL";
		// "Pos ist Element der Liste" wird nicht �berpr�ft (zu aufwendig)

		// aus der Liste ausf�gen:
		if (pPos==m_pBegin)				// kein Vorg�nger: erstes Element l�schen
			m_pBegin = pPos->m_pNext;						// ==NULL falls einziges Element
		else									// es gibt einen Vorg�nger
			pPos->m_pPrev->m_pNext = pPos->m_pNext;	// A: Vorg�nger korrigieren

		if (pPos==m_pEnd)					// kein Nachfolger: letztes Element l�schen
			m_pEnd = pPos->m_pPrev;							// ==NULL falls einziges Element
		else									// es gibt einen Nachfolger
			pPos->m_pNext->m_pPrev = pPos->m_pPrev;	// B: Nachfolger korrigieren		

        // Nutzinhalt retten
		if (removedValue)
		{
			StringCchCopyW (removedValue, MAX_PATH, pPos->m_Value);
		}
		delete [] pPos;
        // Nutzinhalt abliefern
	}
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
