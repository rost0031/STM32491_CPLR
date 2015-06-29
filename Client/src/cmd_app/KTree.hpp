/**
 * @file    KTree.hpp
 * Implementation of a K-ary Tree class.
 *
 * This K-ary tree class is used to model the menu system for the client.
 * # Design of the KTREE algorithm
 * The Menu is built around a k-ary tree (k-tree) which is implemented as a
 * binary tree (b-tree). Unlike a regular k-tree where a node can have multiple
 * children which would make sense for a menu system, since the system is memory
 * and resource constrained, a binary tree makes more since.  It allows for
 * statically allocated nodes without using fixed sized arrays to store child
 * nodes. This implementation is a little confusing at first but is actually
 * used in several resource sensitive systems such as the Linux kernel.
 *
 * A regular k-tree is represented as:
 * struct t {
 *    int n;
 *    int numKids;
 *    struct t **kids;
 * }
 *
 * and the resulting tree struture looks like:
 *
 *  TOP
 *  |
 *  [ list of kids ]
 *  |     |      |
 *  k1    k2     k3
 *  |
 *  |
 *  [list of kids]
 *  |     |     |
 *  k1.1  k1.2  k1.3
 *
 * This representation works if you have things like malloc but on an embedded
 * system where using malloc is anathema, if you want to add another child, use
 * a static array of limited length in addition to having to record the number
 * of kids in the array in the structure.
 *
 * Instead, the following structure is used:
 * struct t {
 *    int n;
 *    struct t *firstChildNode;
 *    struct t *firstSiblingNode;
 *    struct t *trueParentNode;
 *    struct t *fakeParentNode;
 * };
 *
 * and the tree representation looks like:
 *
 * ___________
 * [TOP NODE ] <-
 * -----------   \
 * ^|  ^          \
 * ||  |           \ fP
 * ||  |tP, fP      \
 * ||  |_______  fS  \__________     fS
 * ||  [NODE 2]------>[NODE 2.1]---------->NULL
 * ||  ------- <------ ---------
 * ||       |      tP      |
 * ||       |fC            |fC
 * ||fC     V              V
 * ||      NULL          NULL
 * ||
 * |tP, fP
 * ||
 * |V_____
 * [NODE1]
 * -------
 *    ^
 *    |tP, fp
 *    |_________   fS
 *    [NODE 1.1] -----> NULL
 *        |
 *        |fC
 *        |
 *        V
 *      NULL
 *
 * Where:
 * fP = fakeParentNode - this pointer is used to figure out where the actual
 * parent as it would appear in a classical k-ary tree.
 * tP = trueParentNode - this pointer is used to show where this node is actually
 * attached since it could be either a child node of another node or a sibling.
 * fC = firstChildNode - pointer to the first child node.
 * fS = firstSiblingNode - pointer to the first sibling node.
 *
 * From any node, you can only access the first child or the first sibling,
 * making this a classical b-tree. From that first child, you can access it's
 * children and siblings.  Likewise for the first sibling pointer. This way all
 * the nodes are stored in a list which can be comprised of statically allocated
 * structures.  The true and fake Parent pointers are for easily tracing the
 * ancestry of each child node.  The fake parent pointer is used to map a
 * classical k-tree structure over this b-tree.
 *
 * There are several advantages to this seemingly confusing approach:
 * - Each node is a fixed size and can be statically allocated.
 * - No arrays are needed for representing child nodes.
 * - Many algorithms can be easily expressed since it's just a b-tree.
 *
 * This implementation of the menu is limited in that the functionality to remove
 * nodes is not present since this doesn't seem to be a necessary feature for a
 * menu.  To add a menu or menu item is very simple.  See example in menu_top.c
 *
 * This design was inspired by
 * https://blog.mozilla.org/nnethercote/2012/03/07/n-ary-trees-in-c/
 *
 * @date    06/09/2015
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2015 Datacard. All rights reserved.
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KTREE_HPP_
#define KTREE_HPP_

/* Includes ------------------------------------------------------------------*/
/* System includes */
#include <cstring>
#include <string>
#include <vector>

/* Namespaces ----------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported classes ----------------------------------------------------------*/

/**
 * @class   Ktree
 * @brief   K-ary tree node class this is used for the menu
 */
class Ktree
{

private:
   Ktree* m_parent;
   std::vector<Ktree*> m_children;

public:

   unsigned int m_number;
   std::string m_text;
   std::string m_selector;

   /**
    * @brief   iterator over the private vector
    */
   typedef std::vector<Ktree*>::iterator iterator;

   /**
    * @brief   implementation of the begin() function for the iterator
    */
   iterator children_begin() { return m_children.begin(); }

   /**
    * @brief   implementation of the end() function for the iterator
    */
   iterator children_end() { return m_children.end(); }

   /**
    * @brief   Recursive method to get the current node's depth in the tree.
    * @param [in] currDepth: unsigned int that is passed in from the outside and
    * can be initilized to 0.
    * @return  unsigned int that represents the current node's depth in the tree.
    */
   unsigned int findDepth( unsigned int currDepth );

   /**
    * @brief   Add a child node to current node.
    * @param [in] *child: Ktree pointer to a child node to add.
    * return   None
    */
   void addChild( Ktree* child );

   /**
    * @brief   Create and add a child node to current node.
    * @param [in] selector: const string ref to the selector text to use.
    * @param [in] text: const string ref to the description text to use.
    * return   None
    */
   void addChild(
         const std::string& selector,
         const std::string& text
   );

   /**
    * @brief   Checks if the current node is a leaf (no children)
    * @param   None
    * @return  bool:
    *    @arg true: if leaf (no children)
    *    @arg false: if not a leaf (children exist)
    */
   bool isLeaf( void );

   /**
    * @brief   Find a child node given the selector string
    * @param [in] selector: string ref to the selector text.
    * @return: Ktree* pointer to the found child node.
    */
   Ktree* findChild( const std::string& selector );

   /**
    * @brief   Find a child node given the number of the node
    * @param [in] number: unsigned int that represents the number of the node
    * @return: Ktree* pointer to the found child node.
    */
   Ktree* findChild( unsigned int number );

   /**
    * @brief   Recursive function to find a child in the entire tree.
    *
    * @param [in] number: unsigned int menu node number to find.
    * @return  found node or null.
    */
   Ktree* findChildInTree( const unsigned int number );

   /**
    * @brief   Delete a child node given the selector string
    * @param [in] selector: string ref to the selector text.
    * @return: None
    */
   void deleteChild( const std::string& selector );

   /**
    * @brief   Delete a child node given the number of the node
    * @param [in] number: unsigned int that represents the number of the node
    * @return: None
    */
   void deleteChild( unsigned int number );

   /**
    * @brief   Delete all child of the current node (recursively)
    * @param   None
    * @return: None
    */
   void deleteAllChildren( void );

   /**
    * @brief   Set the current node's parent
    * @param [in] node: Ktree pointer to the parent node to set.
    * return   None
    */
   void setParent( Ktree* node );

   /**
    * @brief   Get the current node's parent
    * @param   None
    * return  Ktree pointer to the parent node
    */
   Ktree* getParent( void );

   /**
    * @brief   Get the ancestry of the current node.
    * @param [in|out]
    */
   void getAncestry( std::vector<Ktree*>& vec );

   /**
    * @brief   Print current node contents
    * @param [in] level: unsigned int current level at which the node should
    * reside at in the tree.
    * @return  None.
    */
   void printNode( unsigned int level );

   /**
    * @brief   Print entire tree contents.
    * @param None
    * @return: none
    */
   void printTree( void );

   /**
    * @brief   Output all the children of the node to stream
    * @param [in|out] ss: stringstream ref to output to.
    * @return  None
    */
   void childrenToStream( std::stringstream& ss );

   /**
    * @brief   Output current node contents to passed in stringstream ref.
    * @param [in|out] ss; stringstream ref to output to.
    * @return: stringstream with appended contents of node.
    */
   void nodeToStream( std::stringstream& ss );

   /**
    * @brief   Output entire tree contents to passed in stringstream ref.
    * @param [in|out] ss; stringstream ref to output to.
    * @return: stringstream with appended contents of tree.
    */
   void treeToStream( std::stringstream& ss );

   /**
    * @brief   Default constructor for a k-ary tree node.
    *
    * @param   None
    * @return  None
    */
   Ktree( void );

   /**
    * @brief   Constructor for a k-ary tree node given a parent node
    *
    * @param   None
    * @return  None
    */
   Ktree(
         const std::string& selector,
         const std::string& text
   );

   /**
    * @brief   Default destructor.
    */
   ~Ktree( void );
};



/**
 * @class   KtreeNav
 * @brief   Navigation helper class to allow printing expanded menu.
 */
class KtreeNav
{
private:

public:

   std::vector<Ktree*> m_ancestry;
   unsigned int m_index;
   unsigned int m_number_of_elems;

   /**
    * @brief   iterator over the private vector
    */
   typedef std::vector<Ktree*>::iterator iterator;

   /**
    * @brief   implementation of the begin() function for the iterator
    */
   iterator ancestry_begin() { return m_ancestry.begin(); }

   /**
    * @brief   implementation of the end() function for the iterator
    */
   iterator ancestry_end() { return m_ancestry.end(); }

   /**
    * @brief   Finds the path to the top of the tree
    */
   void findAncestry( Ktree* node );

   /**
    * @brief   Default constructor.
    *
    * @param   None
    * @return  None
    */
   KtreeNav( void );

   /**
    * @brief   Default destructor.
    * @param   None
    * @return  None
    */
   ~KtreeNav( void );
};

#endif                                                          /* KTREE_HPP_ */
/******** Copyright (C) 2015 Datacard. All rights reserved *****END OF FILE****/
