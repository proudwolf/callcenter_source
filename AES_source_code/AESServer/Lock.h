#ifndef _LOCK_H
#define _LOCK_H

#define _QT_
//#define _VC6_

#ifdef _VC6_
#pragma warning(disable : 4786)
#endif

#include <map>

using namespace std;


template <class KeyType, class ValueType>
class LockMap
{
private:
	map<KeyType, ValueType>							m_Map;

#ifdef _VC6_
    CRITICAL_SECTION								m_cs;
#endif

#ifdef _QT_
    QMutex                                          m_cs;
#endif

    typename map<KeyType, ValueType>::iterator				m_it;
public:
	LockMap()
	{
        m_it = m_Map.end();
#ifdef _VC6_
		InitializeCriticalSection( &m_cs );
#endif
	}

	~LockMap()
	{
#ifdef _VC6_
		DeleteCriticalSection( &m_cs );
#endif
	}

	void Add(const KeyType &Key, const ValueType &Value)
	{
		m_Map.insert( pair<KeyType, ValueType>(Key, Value) );
	}

	void Remove(const KeyType &Key)
	{
		m_Map.erase( Key );
	}

	ValueType Find(const KeyType &Key)
	{
        typename map<KeyType, ValueType>::iterator it = m_Map.find( Key );
		if( it == m_Map.end() )
			return NULL;
		return it->second;
	}

	long Size()
	{
		return m_Map.size();
	}

	void Lock()
	{
#ifdef _VC6_
		EnterCriticalSection( &m_cs );
#endif
#ifdef _QT_
        QMutexLocker locker(&m_cs);
#endif
	}

	void Unlock()
    {
#ifdef _VC6_
		LeaveCriticalSection( &m_cs );
#endif
	}

	void Clear()
	{
		m_Map.clear();
	}

	ValueType Begin()
	{
		m_it = m_Map.begin();
		if( m_it == m_Map.end() )
		{
			return NULL;
		}
		return m_it->second;
	}

	ValueType Next()
	{
		m_it++;
		if(m_it==m_Map.end())
		{
			return NULL;
		}
		return m_it->second;
	}

	KeyType FindKey(ValueType Value)
	{
		if( Size() > 0 )
		{
			for( m_it = m_Map.begin(); m_it != m_Map.end(); ++m_it)
			{
				if( m_it->second == Value )
				{
					return m_it->first;
				}
			}
		}
		return "";
	}
};

#endif
