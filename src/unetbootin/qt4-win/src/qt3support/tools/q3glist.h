/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef Q3GLIST_H
#define Q3GLIST_H

#include <Qt3Support/q3ptrcollection.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3LNode
{
friend class Q3GList;
friend class Q3GListIterator;
friend class Q3GListStdIterator;
public:
    Q3PtrCollection::Item getData()	{ return data; }
private:
    Q3PtrCollection::Item data;
    Q3LNode *prev;
    Q3LNode *next;
    Q3LNode( Q3PtrCollection::Item d ) { data = d; }
};

class Q3GListIteratorList; // internal helper class

class Q_COMPAT_EXPORT Q3GList : public Q3PtrCollection	// doubly linked generic list
{
friend class Q3GListIterator;
friend class Q3GListIteratorList;
friend class Q3GVector;				// needed by Q3GVector::toList
public:
    uint  count() const;			// return number of nodes

#ifndef QT_NO_DATASTREAM
    QDataStream &read( QDataStream & );		// read list from stream
    QDataStream &write( QDataStream & ) const;	// write list to stream
#endif
protected:
    Q3GList();					// create empty list
    Q3GList( const Q3GList & );			// make copy of other list
    virtual ~Q3GList();

    Q3GList &operator=( const Q3GList & );	// assign from other list
    bool operator==( const Q3GList& ) const;

    void inSort( Q3PtrCollection::Item );		// add item sorted in list
    void append( Q3PtrCollection::Item );		// add item at end of list
    bool insertAt( uint index, Q3PtrCollection::Item ); // add item at i'th position
    void relinkNode( Q3LNode * );		// relink as first item
    bool removeNode( Q3LNode * );		// remove node
    bool remove( Q3PtrCollection::Item = 0 );	// remove item (0=current)
    bool removeRef( Q3PtrCollection::Item = 0 );	// remove item (0=current)
    bool removeFirst();				// remove first item
    bool removeLast();				// remove last item
    bool removeAt( uint );			// remove item at i'th position
    bool replaceAt( uint, Q3PtrCollection::Item ); // replace item at position i with item
    Q3PtrCollection::Item takeNode( Q3LNode * );	// take out node
    Q3PtrCollection::Item take();		// take out current item
    Q3PtrCollection::Item takeAt( uint index );	// take out item at i'th pos
    Q3PtrCollection::Item takeFirst();		// take out first item
    Q3PtrCollection::Item takeLast();		// take out last item

    void sort();                        	// sort all items;
    void clear();				// remove all items

    int	 findRef( Q3PtrCollection::Item, bool = true ); // find exact item in list
    int	 find( Q3PtrCollection::Item, bool = true ); // find equal item in list

    uint containsRef( Q3PtrCollection::Item ) const;	// get number of exact matches
    uint contains( Q3PtrCollection::Item ) const;	// get number of equal matches

    Q3PtrCollection::Item at( uint index );	// access item at i'th pos
    int	  at() const;				// get current index
    Q3LNode *currentNode() const;		// get current node

    Q3PtrCollection::Item get() const;		// get current item

    Q3PtrCollection::Item cfirst() const;	// get ptr to first list item
    Q3PtrCollection::Item clast()  const;	// get ptr to last list item
    Q3PtrCollection::Item first();		// set first item in list curr
    Q3PtrCollection::Item last();		// set last item in list curr
    Q3PtrCollection::Item next();		// set next item in list curr
    Q3PtrCollection::Item prev();		// set prev item in list curr

    void  toVector( Q3GVector * ) const;		// put items in vector

    virtual int compareItems( Q3PtrCollection::Item, Q3PtrCollection::Item );

#ifndef QT_NO_DATASTREAM
    virtual QDataStream &read( QDataStream &, Q3PtrCollection::Item & );
    virtual QDataStream &write( QDataStream &, Q3PtrCollection::Item ) const;
#endif

    Q3LNode* begin() const { return firstNode; }
    Q3LNode* end() const { return 0; }
    Q3LNode* erase( Q3LNode* it );

private:
    void  prepend( Q3PtrCollection::Item );	// add item at start of list

    void heapSortPushDown( Q3PtrCollection::Item* heap, int first, int last );

    Q3LNode *firstNode;				// first node
    Q3LNode *lastNode;				// last node
    Q3LNode *curNode;				// current node
    int curIndex;				// current index
    uint numNodes;				// number of nodes
    Q3GListIteratorList *iterators; 		// list of iterators

    Q3LNode *locate( uint );			// get node at i'th pos
    Q3LNode *unlink();				// unlink node
};


inline uint Q3GList::count() const
{
    return numNodes;
}

inline bool Q3GList::removeFirst()
{
    first();
    return remove();
}

inline bool Q3GList::removeLast()
{
    last();
    return remove();
}

inline int Q3GList::at() const
{
    return curIndex;
}

inline Q3PtrCollection::Item Q3GList::at( uint index )
{
    Q3LNode *n = locate( index );
    return n ? n->data : 0;
}

inline Q3LNode *Q3GList::currentNode() const
{
    return curNode;
}

inline Q3PtrCollection::Item Q3GList::get() const
{
    return curNode ? curNode->data : 0;
}

inline Q3PtrCollection::Item Q3GList::cfirst() const
{
    return firstNode ? firstNode->data : 0;
}

inline Q3PtrCollection::Item Q3GList::clast() const
{
    return lastNode ? lastNode->data : 0;
}


/*****************************************************************************
  Q3GList stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_COMPAT_EXPORT QDataStream &operator>>( QDataStream &, Q3GList & );
Q_COMPAT_EXPORT QDataStream &operator<<( QDataStream &, const Q3GList & );
#endif

/*****************************************************************************
  Q3GListIterator class
 *****************************************************************************/

class Q_COMPAT_EXPORT Q3GListIterator			// Q3GList iterator
{
friend class Q3GList;
friend class Q3GListIteratorList;
protected:
    Q3GListIterator( const Q3GList & );
    Q3GListIterator( const Q3GListIterator & );
    Q3GListIterator &operator=( const Q3GListIterator & );
   ~Q3GListIterator();

    bool  atFirst() const;			// test if at first item
    bool  atLast()  const;			// test if at last item
    Q3PtrCollection::Item	  toFirst();				// move to first item
    Q3PtrCollection::Item	  toLast();				// move to last item

    Q3PtrCollection::Item	  get() const;				// get current item
    Q3PtrCollection::Item	  operator()();				// get current and move to next
    Q3PtrCollection::Item	  operator++();				// move to next item (prefix)
    Q3PtrCollection::Item	  operator+=(uint);			// move n positions forward
    Q3PtrCollection::Item	  operator--();				// move to prev item (prefix)
    Q3PtrCollection::Item	  operator-=(uint);			// move n positions backward

protected:
    Q3GList *list;				// reference to list

private:
    Q3LNode  *curNode;				// current node in list
};


inline bool Q3GListIterator::atFirst() const
{
    return curNode == list->firstNode;
}

inline bool Q3GListIterator::atLast() const
{
    return curNode == list->lastNode;
}

inline Q3PtrCollection::Item Q3GListIterator::get() const
{
    return curNode ? curNode->data : 0;
}

class Q_COMPAT_EXPORT Q3GListStdIterator
{
public:
    inline Q3GListStdIterator( Q3LNode* n ) : node( n ){}
    inline operator Q3LNode* () { return node; }
protected:
    inline Q3LNode *next() { return node->next; }
    Q3LNode *node;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif	// Q3GLIST_H
