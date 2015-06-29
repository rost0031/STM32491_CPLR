/**
 * @file    KTree.cpp
 * Implementation of a K-ary Tree class.
 *
 * This K-ary tree class is used to model the menu system for the client.
 *
 * @date    06/09/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Includes ------------------------------------------------------------------*/

/* System includes */
#include <iomanip>
#include <iostream>

/* Boost includes */
#include <boost/algorithm/string.hpp>

/* App includes */
#include "Ktree.hpp"
#include "Logging.hpp"
#include "EnumMaps.hpp"

/* Lib includes */

/* Namespaces ----------------------------------------------------------------*/
using namespace std;

/* Compile-time called macros ------------------------------------------------*/
MODULE_NAME( MODULE_EXT );

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private class prototypes --------------------------------------------------*/
/* Private classes -----------------------------------------------------------*/

/******************************************************************************/
unsigned int Ktree::findDepth( unsigned int currDepth )
{
   if ( NULL == m_parent ) {
      return( currDepth );
   }

   return( m_parent->findDepth( ++currDepth) );
}

/******************************************************************************/
void Ktree::addChild( Ktree* child )
{
   child->setParent( this );
   m_children.push_back( child );
}

/******************************************************************************/
void Ktree::addChild(
      const string& selector,
      const string& text
)
{
   Ktree *node = new Ktree( selector, text );
   node->setParent(this);
   m_children.push_back( node );
}

/******************************************************************************/
bool Ktree::isLeaf( void )
{
   if ( m_children.empty() ) {
      return( true );
   }

   return( false );
}

/******************************************************************************/
Ktree* Ktree::findChild( const string& selector )
{
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      if( boost::iequals((*it)->m_selector, selector) ) {
         return( *it );
      }
   }

   return( NULL );
}

/******************************************************************************/
Ktree* Ktree::findChild( unsigned int number )
{
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      if( (*it)->m_number == number ) {
         return( *it );
      }
   }

   return( NULL );
}


/******************************************************************************/
Ktree* Ktree::findChildInTree( const unsigned int number )
{
   if ( m_number == number ) {
      return this;
   }

   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      Ktree *node = (*it)->findChildInTree( number);
      if (NULL !=  node ) {
         return node;
      }
   }

   return NULL;
}

/******************************************************************************/
void Ktree::deleteChild( const string& selector )
{
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      if( boost::iequals((*it)->m_selector, selector) ) {
         (*it)->deleteAllChildren();
         m_children.erase(it);
         delete (*it);
         return;
      }
   }
}

/******************************************************************************/
void Ktree::deleteChild( unsigned int number )
{
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      if( (*it)->m_number == number ) {
         (*it)->deleteAllChildren();
         m_children.erase(it);
         return;
      }
   }
}

/******************************************************************************/
void Ktree::deleteAllChildren( void )
{
   // recursively call delete on each child.
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      m_children.erase(it);
      delete (*it);
   }

   m_children.clear();
}

/******************************************************************************/
void Ktree::setParent( Ktree* node )
{
   this->m_parent = node;
}

/******************************************************************************/
Ktree* Ktree::getParent( void )
{
   return( m_parent );
}

/******************************************************************************/
void Ktree::printNode( unsigned int level )
{
   stringstream ss;
   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "*--";
   }
   ss << "Text: " << m_text << endl;

   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "*--";
   }
   ss << "Selector: " << m_selector << endl;

   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "*--";
   }
   ss << "Number: " << m_number << endl;

   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "*--";
   }
   ss << "Parent ptr: " << m_parent << endl;

   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "*--";
   }
   ss << "Children ptrs: " << endl;
//   for ( unsigned int i = 0; i < m_children.size(); i++ ) {
//      ss << "Child number " << i <<": " << m_children[i].m_number << endl;
//   }
   CON_print(ss.str());
}

/******************************************************************************/
void Ktree::printTree( void )
{
   stringstream ss;
   this->treeToStream( ss );
   CON_print(ss.str());
}

/******************************************************************************/
void Ktree::childrenToStream( std::stringstream& ss )
{
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      (*it)->nodeToStream( ss );
   }
}

/******************************************************************************/
void Ktree::nodeToStream( stringstream& ss )
{
   unsigned int level = this->findDepth(0);
   for ( unsigned int i = 0; i < level; i++ ) {
      ss << "   ";
   }

   ss << "*--";
   ss << "** " << std::setw(3) << m_selector << " (" << m_number << ") ** : " << m_text << endl;
}

/******************************************************************************/
void Ktree::treeToStream( stringstream& ss )
{
   this->nodeToStream(ss);
   vector<Ktree*>::iterator it = m_children.begin();
   for( it = m_children.begin(); it != m_children.end(); ++it ) {
      (*it)->treeToStream( ss );
   }
}

/******************************************************************************/
Ktree::Ktree( void ) :
      m_parent(NULL),
      m_number(0),
      m_text(""),
      m_selector("")
{

}

/******************************************************************************/
Ktree::Ktree(
      const std::string& selector,
      const std::string& text
) :
      m_parent(NULL),
      m_number(0),
      m_text(""),
      m_selector("")
{
   m_text = text;
   m_selector = selector;
}

/******************************************************************************/
Ktree::~Ktree( void )
{
   m_parent = NULL;
   this->deleteAllChildren();
   m_number = 0;
   m_text.clear();
   m_selector.clear();
}


/******************************************************************************/
void KtreeNav::findAncestry( Ktree* node )
{
   m_ancestry.insert(m_ancestry.begin(), node);
   ++m_number_of_elems;
   if (node->getParent()) {
      findAncestry(node->getParent());
   }
   return;
}

/******************************************************************************/
KtreeNav::KtreeNav( void ) :
      m_ancestry(0),
      m_index(0),
      m_number_of_elems(0)
{

}

/******************************************************************************/
KtreeNav::~KtreeNav( void )
{
   m_ancestry.clear();
   m_index = 0;
   m_number_of_elems = 0;
}

/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
