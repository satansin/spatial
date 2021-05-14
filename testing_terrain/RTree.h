#ifndef RTREE_H
#define RTREE_H

// NOTE This file compiles under MSVC 6 SP5 and MSVC .Net 2003 it may not work on other compilers without modification.

// NOTE These next few lines may be win32 specific, you may need to modify them to compile on other platform
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>

#include <vector>

#include <iostream>
#include <limits>

#include <Eigen/Dense>

#define ASSERT assert // RTree uses ASSERT( condition )
#ifndef Min
  #define Min std::min
#endif //Min
#ifndef Max
  #define Max std::max
#endif //Max

//
// RTree.h
//

#define RTREE_TEMPLATE template<class DATATYPE, class ELEMTYPE, int NUMDIMS, class ELEMTYPEREAL, int TMAXNODES, int TMINNODES>
#define RTREE_QUAL RTree<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>

#define RTREE_TEMPLATE_PRIME template<class DATATYPE_P, class ELEMTYPE_P, int NUMDIMS_P, class ELEMTYPEREAL_P, int TMAXNODES_P, int TMINNODES_P>
#define RTREE_QUAL_PRIME RTree<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define OUTRA_RECT OutraRect<ELEMTYPE, NUMDIMS>

#define OVERLAP_P Overlap<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define OVERLAPPING_AREA_P OverlappingArea<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define RESIDUAL_SEARCH   ResidualSearch<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>
#define RESIDUAL_SEARCH_P ResidualSearch<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define JOIN_NODE_P JoinNode<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define JOIN_INT_NODE   JoinInternalLoop<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>
#define JOIN_INT_NODE_P JoinInternalLoop<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define SORTED_RESIDUAL_SEARCH   SortedResidualSearch<DATATYPE, ELEMTYPE, NUMDIMS, ELEMTYPEREAL, TMAXNODES, TMINNODES>
#define SORTED_RESIDUAL_SEARCH_P SortedResidualSearch<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define JOIN_SORTED_NODE_P JoinSortedNode<DATATYPE_P, ELEMTYPE_P, NUMDIMS_P, ELEMTYPEREAL_P, TMAXNODES_P, TMINNODES_P>

#define RTREE_DONT_USE_MEMPOOLS // This version does not contain a fixed memory allocator, fill in lines with EXAMPLE to implement one.
#define RTREE_USE_SPHERICAL_VOLUME // Better split classification, may be slower on some systems

// Fwd decl
class RTFileStream;  // File I/O helper class, look below for implementation and notes.

template<class ELEMTYPE, int NUMDIMS>
struct OutraRect
{
  ELEMTYPE m_min[NUMDIMS];
  ELEMTYPE m_max[NUMDIMS];
};


/// \class RTree
/// Implementation of RTree, a multidimensional bounding rectangle tree.
/// Example usage: For a 3-dimensional tree use RTree<Object*, float, 3> myTree;
///
/// This modified, templated C++ version by Greg Douglas at Auran (http://www.auran.com)
///
/// DATATYPE Referenced data, should be int, void*, obj* etc. no larger than sizeof<void*> and simple type
/// ELEMTYPE Type of element such as int or float
/// NUMDIMS Number of dimensions such as 2 or 3
/// ELEMTYPEREAL Type of element that allows fractional and large values such as float or double, for use in volume calcs
///
/// NOTES: Inserting and removing data requires the knowledge of its constant Minimal Bounding Rectangle.
///        This version uses new/delete for nodes, I recommend using a fixed size allocator for efficiency.
///        Instead of using a callback function for returned results, I recommend and efficient pre-sized, grow-only memory
///        array similar to MFC CArray or STL Vector for returning search query result.
///
template<class DATATYPE, class ELEMTYPE, int NUMDIMS, 
         class ELEMTYPEREAL = ELEMTYPE, int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>
class RTree
{

protected:

  struct Node;  // Fwd decl.  Used by other internal structs and iterator

public:

  // These constant must be declared after Branch and before Node struct
  // Stuck up here for MSVC 6 compiler.  NSVC .NET 2003 is much happier.
  enum
  {
    MAXNODES = TMAXNODES,                         ///< Max elements in node
    MINNODES = TMINNODES,                         ///< Min elements in node
  };

  RTree();
  RTree(const RTree& other);
  virtual ~RTree();

  /// Insert entry
  /// \param a_min Min of bounding rect
  /// \param a_max Max of bounding rect
  /// \param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
  void Insert(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const DATATYPE& a_dataId);

  void InsertVertex(const ELEMTYPE a_vertex[NUMDIMS], const DATATYPE& a_dataId);
  void InsertFace(const ELEMTYPE a_triver[3][NUMDIMS], const DATATYPE& a_dataId);
  
  /// Remove entry
  /// \param a_min Min of bounding rect
  /// \param a_max Max of bounding rect
  /// \param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
  void Remove(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const DATATYPE& a_dataId);

  /// Sort branches of all the nodes by the first dimension
  void SortDim0();
  
  /// Find all within search rectangle
  /// \param a_min Min of search bounding rect
  /// \param a_max Max of search bounding rect
  /// \param a_searchResult Search result array.  Caller should set grow size. Function will reset, not append to array.
  /// \param a_resultCallback Callback function to return result.  Callback should return 'true' to continue searching
  /// \param a_context User context to pass as parameter to a_resultCallback
  /// \return Returns the number of entries found
  int Search(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], std::vector<DATATYPE>& a_searchResult) const;

  bool YoNSearch(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS]) const;

  int Search(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const Node* node, std::vector<DATATYPE>& a_searchResult) const;

  int NodeSearch(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], std::vector<Node*>& ret, int stop_level) const;

  RTREE_TEMPLATE_PRIME
  int SpatialJoin(const RTREE_QUAL_PRIME* tree_prime, std::vector<std::pair<int, int>>& ret) const;

  int SphereSearch(const ELEMTYPE center[NUMDIMS], ELEMTYPEREAL sq_dist_min, ELEMTYPEREAL sq_dist_max, std::vector<DATATYPE>& ret) const;

  DATATYPE nnVertexSearch(const ELEMTYPE center[NUMDIMS]) const;
  void knnVertexSearch(const ELEMTYPE center[NUMDIMS], int k, std::vector<DATATYPE>& ret) const;
  
  /// Remove all entries from tree
  void RemoveAll();

  /// Count the data elements in this container.  This is slow as no internal counter is maintained.
  int Count();

  /// Load tree contents from file
  bool Load(const char* a_fileName);
  /// Load tree contents from stream
  bool Load(RTFileStream& a_stream);

  
  /// Save tree contents to file
  bool Save(const char* a_fileName);
  /// Save tree contents to stream
  bool Save(RTFileStream& a_stream);

  /// Iterator is not remove safe.
  class Iterator
  {
  private:
  
    enum { MAX_STACK = 32 }; //  Max stack size. Allows almost n^32 where n is number of branches in node
    
    struct StackElement
    {
      Node* m_node;
      int m_branchIndex;
    };
    
  public:
  
    Iterator()                                    { Init(); }

    ~Iterator()                                   { }
    
    /// Is iterator invalid
    bool IsNull()                                 { return (m_tos <= 0); }

    /// Is iterator pointing to valid data
    bool IsNotNull()                              { return (m_tos > 0); }

    /// Access the current data element. Caller must be sure iterator is not NULL first.
    DATATYPE& operator*()
    {
      ASSERT(IsNotNull());
      StackElement& curTos = m_stack[m_tos - 1];
      return curTos.m_node->m_branch[curTos.m_branchIndex].m_data;
    } 

    /// Access the current data element. Caller must be sure iterator is not NULL first.
    const DATATYPE& operator*() const
    {
      ASSERT(IsNotNull());
      StackElement& curTos = m_stack[m_tos - 1];
      return curTos.m_node->m_branch[curTos.m_branchIndex].m_data;
    } 

    /// Find the next data element
    bool operator++()                             { return FindNextData(); }

    /// Get the bounds for this node
    void GetBounds(ELEMTYPE a_min[NUMDIMS], ELEMTYPE a_max[NUMDIMS])
    {
      ASSERT(IsNotNull());
      StackElement& curTos = m_stack[m_tos - 1];
      Branch& curBranch = curTos.m_node->m_branch[curTos.m_branchIndex];
      
      for(int index = 0; index < NUMDIMS; ++index)
      {
        a_min[index] = curBranch.m_rect.m_min[index];
        a_max[index] = curBranch.m_rect.m_max[index];
      }
    }

  private:
  
    /// Reset iterator
    void Init()                                   { m_tos = 0; }

    /// Find the next data element in the tree (For internal use only)
    bool FindNextData()
    {
      for(;;)
      {
        if(m_tos <= 0)
        {
          return false;
        }
        StackElement curTos = Pop(); // Copy stack top cause it may change as we use it

        if(curTos.m_node->IsLeaf())
        {
          // Keep walking through data while we can
          if(curTos.m_branchIndex+1 < curTos.m_node->m_count)
          {
            // There is more data, just point to the next one
            Push(curTos.m_node, curTos.m_branchIndex + 1);
            return true;
          }
          // No more data, so it will fall back to previous level
        }
        else
        {
          if(curTos.m_branchIndex+1 < curTos.m_node->m_count)
          {
            // Push sibling on for future tree walk
            // This is the 'fall back' node when we finish with the current level
            Push(curTos.m_node, curTos.m_branchIndex + 1);
          }
          // Since cur node is not a leaf, push first of next level to get deeper into the tree
          Node* nextLevelnode = curTos.m_node->m_branch[curTos.m_branchIndex].m_child;
          Push(nextLevelnode, 0);
          
          // If we pushed on a new leaf, exit as the data is ready at TOS
          if(nextLevelnode->IsLeaf())
          {
            return true;
          }
        }
      }
    }

    /// Push node and branch onto iteration stack (For internal use only)
    void Push(Node* a_node, int a_branchIndex)
    {
      m_stack[m_tos].m_node = a_node;
      m_stack[m_tos].m_branchIndex = a_branchIndex;
      ++m_tos;
      ASSERT(m_tos <= MAX_STACK);
    }
    
    /// Pop element off iteration stack (For internal use only)
    StackElement& Pop()
    {
      ASSERT(m_tos > 0);
      --m_tos;
      return m_stack[m_tos];
    }

    StackElement m_stack[MAX_STACK];              ///< Stack as we are doing iteration instead of recursion
    int m_tos;                                    ///< Top Of Stack index
  
    friend class RTree; // Allow hiding of non-public functions while allowing manipulation by logical owner
  };

  /// Get 'first' for iteration
  void GetFirst(Iterator& a_it)
  {
    a_it.Init();
    Node* first = m_root;
    while(first)
    {
      if(first->IsInternalNode() && first->m_count > 1)
      {
        a_it.Push(first, 1); // Descend sibling branch later
      }
      else if(first->IsLeaf())
      {
        if(first->m_count)
        {
          a_it.Push(first, 0);
        }
        break;
      }
      first = first->m_branch[0].m_child;
    }
  }  

  /// Get Next for iteration
  void GetNext(Iterator& a_it)                    { ++a_it; }

  /// Is iterator NULL, or at end?
  bool IsNull(Iterator& a_it)                     { return a_it.IsNull(); }

  /// Get object at iterator position
  DATATYPE& GetAt(Iterator& a_it)                 { return *a_it; }

protected:

  /// Minimal bounding rectangle (n-dimensional)
  struct Rect
  {
    ELEMTYPE m_min[NUMDIMS];                      ///< Min dimensions of bounding box 
    ELEMTYPE m_max[NUMDIMS];                      ///< Max dimensions of bounding box 
  };

  struct Tri
  {
    ELEMTYPE m_ver[3][NUMDIMS];
  };

  /// May be data or may be another subtree
  /// The parents level determines this.
  /// If the parents level is 0, then this is data
  struct Branch
  {
    Rect m_rect;                                  ///< Bounds
    Node* m_child;                                ///< Child node
    DATATYPE m_data;                              ///< Data Id
    bool m_is_tri;
    Tri m_tri;
  };

  /// Node for each branch level
  struct Node
  {
    bool IsInternalNode() const                   { return (m_level > 0); } // Not a leaf, but a internal node
    bool IsLeaf() const                           { return (m_level == 0); } // A leaf, contains data
    
    int m_count;                                  ///< Count
    int m_level;                                  ///< Leaf is zero, others positive
    Branch m_branch[MAXNODES];                    ///< Branch
  };
  
  /// A link list of nodes for reinsertion after a delete operation
  struct ListNode
  {
    ListNode* m_next;                             ///< Next in list
    Node* m_node;                                 ///< Node
  };

  /// Variables for finding a split partition
  struct PartitionVars
  {
    enum { NOT_TAKEN = -1 }; // indicates that position

    int m_partition[MAXNODES+1];
    int m_total;
    int m_minFill;
    int m_count[2];
    Rect m_cover[2];
    ELEMTYPEREAL m_area[2];

    Branch m_branchBuf[MAXNODES+1];
    int m_branchCount;
    Rect m_coverSplit;
    ELEMTYPEREAL m_coverSplitArea;
  };
 
  Node* AllocNode();
  void FreeNode(Node* a_node);
  void InitNode(Node* a_node);
  void InitRect(Rect* a_rect);
  bool InsertRectRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level);
  bool InsertRect(const Branch& a_branch, Node** a_root, int a_level);
  Rect NodeCover(Node* a_node);
  bool AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode);
  void DisconnectBranch(Node* a_node, int a_index);
  int PickBranch(const Rect* a_rect, Node* a_node);
  Rect CombineRect(const Rect* a_rectA, const Rect* a_rectB);
  void SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode);
  ELEMTYPEREAL RectSphericalVolume(Rect* a_rect);
  ELEMTYPEREAL RectVolume(Rect* a_rect);
  ELEMTYPEREAL CalcRectVolume(Rect* a_rect);
  void GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars);
  void ChoosePartition(PartitionVars* a_parVars, int a_minFill);
  void LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars);
  void InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill);
  void PickSeeds(PartitionVars* a_parVars);
  void Classify(int a_index, int a_group, PartitionVars* a_parVars);
  bool RemoveRect(Rect* a_rect, const DATATYPE& a_id, Node** a_root);
  bool RemoveRectRec(Rect* a_rect, const DATATYPE& a_id, Node* a_node, ListNode** a_listNode);
  ListNode* AllocListNode();
  void FreeListNode(ListNode* a_listNode);
  bool Overlap(const Rect* a_rectA, const Rect* a_rectB) const;
  void ReInsert(Node* a_node, ListNode** a_listNode);
  bool Search(Node* a_node, Rect* a_rect, int& a_foundCount, std::vector<DATATYPE>& ret) const;
  bool YoNSearch(Node* a_node, Rect* a_rect) const;
  bool NodeSearch(Node* a_node, Rect* a_rect, int& a_foundCount, std::vector<Node*>& ret, int stop_level) const;
  void RemoveAllRec(Node* a_node);
  void Reset();
  void CountRec(Node* a_node, int& a_count);

  bool SaveRec(Node* a_node, RTFileStream& a_stream);
  bool LoadRec(Node* a_node, RTFileStream& a_stream);
  void CopyRec(Node* current, Node* other);

  Node* m_root;                                    ///< Root of tree
  ELEMTYPEREAL m_unitSphereVolume;                 ///< Unit sphere constant for required number of dimensions

  RTREE_TEMPLATE_PRIME friend class RTree;

  // bool CompBranch(const Branch& a, const Branch& b);
  void SortDim0Node(Node* a_node);

  RTREE_TEMPLATE_PRIME
  bool Overlap(const Rect* rect, const typename RTREE_QUAL_PRIME::Rect* rect_prime) const;

  bool Overlap(const Rect* rect, const OUTRA_RECT* ovlp_rect) const;

  RTREE_TEMPLATE_PRIME
  void OverlappingArea(const Rect* rect, const typename RTREE_QUAL_PRIME::Rect* rect_prime, OUTRA_RECT& ret) const;

  RTREE_TEMPLATE_PRIME
  int ResidualSearch(const Node* node, const typename RTREE_QUAL_PRIME::Rect* rect_prime, const OUTRA_RECT* ovlp_rect, std::vector<int>& ret) const;

  RTREE_TEMPLATE_PRIME
  int JoinNode(const Node* node, const RTREE_QUAL_PRIME* tree_prime, const typename RTREE_QUAL_PRIME::Node* node_prime, const OUTRA_RECT* ovlp_rect, std::vector<std::pair<int, int>>& ret) const;

	RTREE_TEMPLATE_PRIME
	int SortedResidualSearch(const Node* node, const typename RTREE_QUAL_PRIME::Rect* rect_prime, const OUTRA_RECT* ovlp_rect, std::vector<int>& ret) const;

	RTREE_TEMPLATE_PRIME
	int JoinInternalLoop(const Rect* rect, int unmarked, const typename RTREE_QUAL_PRIME::Node* node_prime, std::vector<int>& ret) const;

	RTREE_TEMPLATE_PRIME
	int JoinSortedNode(const Node* node, const RTREE_QUAL_PRIME* tree_prime, const typename RTREE_QUAL_PRIME::Node* node_prime, const OUTRA_RECT* ovlp_rect, std::vector<std::pair<int, int>>& ret) const;

  int SphereSearchNode(const Node* node, const ELEMTYPE center[NUMDIMS], ELEMTYPEREAL sq_min, ELEMTYPEREAL sq_max, std::vector<DATATYPE>& ret) const;

  void knnVertexSearchNode(const Node* node, const ELEMTYPE center[NUMDIMS], int k, std::vector<DATATYPE>& ret, std::vector<ELEMTYPEREAL>& sq_dist_arr) const;

  ELEMTYPEREAL SqDist(const ELEMTYPE center[NUMDIMS], const Branch* a_branch) const;
  ELEMTYPEREAL SqDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const;
  ELEMTYPEREAL SqDist(const ELEMTYPE p[NUMDIMS], const ELEMTYPE q[NUMDIMS]) const;
  ELEMTYPEREAL SqMinDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const;
  ELEMTYPEREAL SqMaxDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const;

  bool CheckPtProjInsideTri(const ELEMTYPE center[NUMDIMS], const Tri* a_tri, ELEMTYPE proj[NUMDIMS]) const;
  ELEMTYPEREAL SqDistPtLine(const ELEMTYPE q[NUMDIMS], const ELEMTYPE s[NUMDIMS], const ELEMTYPE t[NUMDIMS], double& lambda) const;
  ELEMTYPEREAL SqDistPtTri(const ELEMTYPE center[NUMDIMS], const Tri* a_tri) const;


};


// Because there is not stream support, this is a quick and dirty file I/O helper.
// Users will likely replace its usage with a Stream implementation from their favorite API.
class RTFileStream
{
  FILE* m_file;

public:

  
  RTFileStream()
  {
    m_file = NULL;
  }

  ~RTFileStream()
  {
    Close();
  }

  bool OpenRead(const char* a_fileName)
  {
    m_file = fopen(a_fileName, "rb");
    if(!m_file)
    {
      return false;
    }
    return true;
  }

  bool OpenWrite(const char* a_fileName)
  {
    m_file = fopen(a_fileName, "wb");
    if(!m_file)
    {
      return false;
    }
    return true;
  }

  void Close()
  {
    if(m_file)
    {
      fclose(m_file);
      m_file = NULL;
    }
  }

  template< typename TYPE >
  size_t Write(const TYPE& a_value)
  {
    ASSERT(m_file);
    return fwrite((void*)&a_value, sizeof(a_value), 1, m_file);
  }

  template< typename TYPE >
  size_t WriteArray(const TYPE* a_array, int a_count)
  {
    ASSERT(m_file);
    return fwrite((void*)a_array, sizeof(TYPE) * a_count, 1, m_file);
  }

  template< typename TYPE >
  size_t Read(TYPE& a_value)
  {
    ASSERT(m_file);
    return fread((void*)&a_value, sizeof(a_value), 1, m_file);
  }

  template< typename TYPE >
  size_t ReadArray(TYPE* a_array, int a_count)
  {
    ASSERT(m_file);
    return fread((void*)a_array, sizeof(TYPE) * a_count, 1, m_file);
  }
};


RTREE_TEMPLATE
RTREE_QUAL::RTree()
{
  ASSERT(MAXNODES > MINNODES);
  ASSERT(MINNODES > 0);

  // Precomputed volumes of the unit spheres for the first few dimensions
  const float UNIT_SPHERE_VOLUMES[] = {
    0.000000f, 2.000000f, 3.141593f, // Dimension  0,1,2
    4.188790f, 4.934802f, 5.263789f, // Dimension  3,4,5
    5.167713f, 4.724766f, 4.058712f, // Dimension  6,7,8
    3.298509f, 2.550164f, 1.884104f, // Dimension  9,10,11
    1.335263f, 0.910629f, 0.599265f, // Dimension  12,13,14
    0.381443f, 0.235331f, 0.140981f, // Dimension  15,16,17
    0.082146f, 0.046622f, 0.025807f, // Dimension  18,19,20 
  };

  m_root = AllocNode();
  m_root->m_level = 0;
  m_unitSphereVolume = (ELEMTYPEREAL)UNIT_SPHERE_VOLUMES[NUMDIMS];
}


RTREE_TEMPLATE
RTREE_QUAL::RTree(const RTree& other) : RTree()
{
	CopyRec(m_root, other.m_root);
}


RTREE_TEMPLATE
RTREE_QUAL::~RTree()
{
  Reset(); // Free, or reset node memory
}


RTREE_TEMPLATE
void RTREE_QUAL::Insert(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const DATATYPE& a_dataId)
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Branch branch;
  branch.m_data = a_dataId;
  branch.m_child = NULL;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    branch.m_rect.m_min[axis] = a_min[axis];
    branch.m_rect.m_max[axis] = a_max[axis];
  }
  
  InsertRect(branch, &m_root, 0);
}


RTREE_TEMPLATE
void RTREE_QUAL::InsertVertex(const ELEMTYPE a_vertex[NUMDIMS], const DATATYPE& a_dataId)
{
  Branch branch;
  branch.m_data = a_dataId;
  branch.m_child = NULL;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    branch.m_rect.m_min[axis] = a_vertex[axis];
    branch.m_rect.m_max[axis] = a_vertex[axis];
  }

  branch.m_is_tri = false;
  
  InsertRect(branch, &m_root, 0);
}


RTREE_TEMPLATE
void RTREE_QUAL::InsertFace(const ELEMTYPE a_triver[3][NUMDIMS], const DATATYPE& a_dataId)
{
  Branch branch;
  branch.m_data = a_dataId;
  branch.m_child = NULL;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    branch.m_rect.m_min[axis] = Min({a_triver[0][axis], a_triver[1][axis], a_triver[2][axis]});
    branch.m_rect.m_max[axis] = Max({a_triver[0][axis], a_triver[1][axis], a_triver[2][axis]});
  }

  branch.m_is_tri = true;
  for (int i = 0; i < 3; i++) {
    for (int axis = 0; axis < NUMDIMS; axis++) {
      branch.m_tri.m_ver[i][axis] = a_triver[i][axis];
    }
  }
  
  InsertRect(branch, &m_root, 0);
}


RTREE_TEMPLATE
void RTREE_QUAL::Remove(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const DATATYPE& a_dataId)
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Rect rect;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    rect.m_min[axis] = a_min[axis];
    rect.m_max[axis] = a_max[axis];
  }

  RemoveRect(&rect, a_dataId, &m_root);
}

RTREE_TEMPLATE
void RTREE_QUAL::SortDim0()
{
  SortDim0Node(m_root);
}

RTREE_TEMPLATE
bool RTREE_QUAL::YoNSearch(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS]) const
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Rect rect;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    rect.m_min[axis] = a_min[axis];
    rect.m_max[axis] = a_max[axis];
  }

  return YoNSearch(m_root, &rect);
}

RTREE_TEMPLATE
int RTREE_QUAL::Search(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], std::vector<DATATYPE>& ret) const
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Rect rect;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    rect.m_min[axis] = a_min[axis];
    rect.m_max[axis] = a_max[axis];
  }

  int foundCount = 0;
  Search(m_root, &rect, foundCount, ret);

  return foundCount;
}

RTREE_TEMPLATE
int RTREE_QUAL::Search(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], const Node* node, std::vector<DATATYPE>& ret) const
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Rect rect;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    rect.m_min[axis] = a_min[axis];
    rect.m_max[axis] = a_max[axis];
  }

  int foundCount = 0;
  Search(node, &rect, foundCount, ret);

  return foundCount;
}

RTREE_TEMPLATE
int RTREE_QUAL::NodeSearch(const ELEMTYPE a_min[NUMDIMS], const ELEMTYPE a_max[NUMDIMS], std::vector<Node*>& ret, int stop_level) const
{
#ifdef _DEBUG
  for(int index=0; index<NUMDIMS; ++index)
  {
    ASSERT(a_min[index] <= a_max[index]);
  }
#endif //_DEBUG

  Rect rect;
  
  for(int axis=0; axis<NUMDIMS; ++axis)
  {
    rect.m_min[axis] = a_min[axis];
    rect.m_max[axis] = a_max[axis];
  }

  int foundCount = 0;
  NodeSearch(m_root, &rect, foundCount, ret, stop_level);

  return foundCount;

}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::SpatialJoin(const RTREE_QUAL_PRIME* tree_prime, std::vector<std::pair<int, int>>& ret) const
{
  auto root_prime = tree_prime->m_root;
  std::cout << "Highest of root: " << m_root->m_level << std::endl;
  std::cout << "Highest of aux_root: " << root_prime->m_level << std::endl;

  OUTRA_RECT ovlp_rect;
  for (int i = 0; i < NUMDIMS; i++) {
     ovlp_rect.m_min[i] = std::numeric_limits<ELEMTYPE>::min();
     ovlp_rect.m_max[i] = std::numeric_limits<ELEMTYPE>::max();
  }

  JOIN_SORTED_NODE_P(m_root, tree_prime, root_prime, &ovlp_rect, ret);
  // JOIN_NODE_P(m_root, tree_prime, root_prime, &ovlp_rect, ret);

  return ret.size();
}

RTREE_TEMPLATE
int RTREE_QUAL::SphereSearch(const ELEMTYPE center[NUMDIMS], ELEMTYPEREAL sq_dist_min, ELEMTYPEREAL sq_dist_max, std::vector<DATATYPE>& ret) const
{
  assert(sq_dist_min <= sq_dist_max);

  // ELEMTYPEREAL sq_dist_min = dist_min * dist_min - 0.001;
  // ELEMTYPEREAL sq_dist_max = dist_max * dist_max + 0.001;

  SphereSearchNode(m_root, center, sq_dist_min, sq_dist_max, ret);
}

RTREE_TEMPLATE
DATATYPE RTREE_QUAL::nnVertexSearch(const ELEMTYPE center[NUMDIMS]) const
{
  std::vector<DATATYPE> knn_ret;
  knnVertexSearch(center, 1, knn_ret);
  return knn_ret[0];
}

RTREE_TEMPLATE
void RTREE_QUAL::knnVertexSearch(const ELEMTYPE center[NUMDIMS], int k, std::vector<DATATYPE>& ret) const
{
  assert(k >= 0);
  if (k == 0) {
    return;
  }

  ret.clear();
  std::vector<ELEMTYPEREAL> sq_dist_arr(k);
  for (int i = 0; i < k; i++) {
    ret.push_back(-1);
    sq_dist_arr[i] = std::numeric_limits<ELEMTYPEREAL>::max();
  }

  knnVertexSearchNode(m_root, center, k, ret, sq_dist_arr);
}


RTREE_TEMPLATE
int RTREE_QUAL::Count()
{
  int count = 0;
  CountRec(m_root, count);
  
  return count;
}



RTREE_TEMPLATE
void RTREE_QUAL::CountRec(Node* a_node, int& a_count)
{
  if(a_node->IsInternalNode())  // not a leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      CountRec(a_node->m_branch[index].m_child, a_count);
    }
  }
  else // A leaf node
  {
    a_count += a_node->m_count;
  }
}


RTREE_TEMPLATE
bool RTREE_QUAL::Load(const char* a_fileName)
{
  RemoveAll(); // Clear existing tree

  RTFileStream stream;
  if(!stream.OpenRead(a_fileName))
  {
    return false;
  }

  bool result = Load(stream);
  
  stream.Close();

  return result;
}



RTREE_TEMPLATE
bool RTREE_QUAL::Load(RTFileStream& a_stream)
{
  // Write some kind of header
  int _dataFileId = ('R'<<0)|('T'<<8)|('R'<<16)|('E'<<24);
  int _dataSize = sizeof(DATATYPE);
  int _dataNumDims = NUMDIMS;
  int _dataElemSize = sizeof(ELEMTYPE);
  int _dataElemRealSize = sizeof(ELEMTYPEREAL);
  int _dataMaxNodes = TMAXNODES;
  int _dataMinNodes = TMINNODES;

  int dataFileId = 0;
  int dataSize = 0;
  int dataNumDims = 0;
  int dataElemSize = 0;
  int dataElemRealSize = 0;
  int dataMaxNodes = 0;
  int dataMinNodes = 0;

  a_stream.Read(dataFileId);
  a_stream.Read(dataSize);
  a_stream.Read(dataNumDims);
  a_stream.Read(dataElemSize);
  a_stream.Read(dataElemRealSize);
  a_stream.Read(dataMaxNodes);
  a_stream.Read(dataMinNodes);

  bool result = false;

  // Test if header was valid and compatible
  if(    (dataFileId == _dataFileId) 
      && (dataSize == _dataSize) 
      && (dataNumDims == _dataNumDims) 
      && (dataElemSize == _dataElemSize) 
      && (dataElemRealSize == _dataElemRealSize) 
      && (dataMaxNodes == _dataMaxNodes) 
      && (dataMinNodes == _dataMinNodes) 
    )
  {
    // Recursively load tree
    result = LoadRec(m_root, a_stream);
  }

  return result;
}


RTREE_TEMPLATE
bool RTREE_QUAL::LoadRec(Node* a_node, RTFileStream& a_stream)
{
  a_stream.Read(a_node->m_level);
  a_stream.Read(a_node->m_count);

  if(a_node->IsInternalNode())  // not a leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      Branch* curBranch = &a_node->m_branch[index];

      a_stream.ReadArray(curBranch->m_rect.m_min, NUMDIMS);
      a_stream.ReadArray(curBranch->m_rect.m_max, NUMDIMS);

      curBranch->m_child = AllocNode();
      LoadRec(curBranch->m_child, a_stream);
    }
  }
  else // A leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      Branch* curBranch = &a_node->m_branch[index];

      a_stream.ReadArray(curBranch->m_rect.m_min, NUMDIMS);
      a_stream.ReadArray(curBranch->m_rect.m_max, NUMDIMS);

      a_stream.Read(curBranch->m_data);
    }
  }

  return true; // Should do more error checking on I/O operations
}


RTREE_TEMPLATE
void RTREE_QUAL::CopyRec(Node* current, Node* other)
{
  current->m_level = other->m_level;
  current->m_count = other->m_count;

  if(current->IsInternalNode())  // not a leaf node
  {
    for(int index = 0; index < current->m_count; ++index)
    {
      Branch* currentBranch = &current->m_branch[index];
      Branch* otherBranch = &other->m_branch[index];

      std::copy(otherBranch->m_rect.m_min,
                otherBranch->m_rect.m_min + NUMDIMS,
                currentBranch->m_rect.m_min);

      std::copy(otherBranch->m_rect.m_max,
                otherBranch->m_rect.m_max + NUMDIMS,
                currentBranch->m_rect.m_max);

      currentBranch->m_child = AllocNode();
      CopyRec(currentBranch->m_child, otherBranch->m_child);
    }
  }
  else // A leaf node
  {
    for(int index = 0; index < current->m_count; ++index)
    {
      Branch* currentBranch = &current->m_branch[index];
      Branch* otherBranch = &other->m_branch[index];

      std::copy(otherBranch->m_rect.m_min,
                otherBranch->m_rect.m_min + NUMDIMS,
                currentBranch->m_rect.m_min);

      std::copy(otherBranch->m_rect.m_max,
                otherBranch->m_rect.m_max + NUMDIMS,
                currentBranch->m_rect.m_max);

      currentBranch->m_data = otherBranch->m_data;
    }
  }
}


RTREE_TEMPLATE
bool RTREE_QUAL::Save(const char* a_fileName)
{
  RTFileStream stream;
  if(!stream.OpenWrite(a_fileName))
  {
    return false;
  }

  bool result = Save(stream);

  stream.Close();

  return result;
}


RTREE_TEMPLATE
bool RTREE_QUAL::Save(RTFileStream& a_stream)
{
  // Write some kind of header
  int dataFileId = ('R'<<0)|('T'<<8)|('R'<<16)|('E'<<24);
  int dataSize = sizeof(DATATYPE);
  int dataNumDims = NUMDIMS;
  int dataElemSize = sizeof(ELEMTYPE);
  int dataElemRealSize = sizeof(ELEMTYPEREAL);
  int dataMaxNodes = TMAXNODES;
  int dataMinNodes = TMINNODES;

  a_stream.Write(dataFileId);
  a_stream.Write(dataSize);
  a_stream.Write(dataNumDims);
  a_stream.Write(dataElemSize);
  a_stream.Write(dataElemRealSize);
  a_stream.Write(dataMaxNodes);
  a_stream.Write(dataMinNodes);

  // Recursively save tree
  bool result = SaveRec(m_root, a_stream);
  
  return result;
}


RTREE_TEMPLATE
bool RTREE_QUAL::SaveRec(Node* a_node, RTFileStream& a_stream)
{
  a_stream.Write(a_node->m_level);
  a_stream.Write(a_node->m_count);

  if(a_node->IsInternalNode())  // not a leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      Branch* curBranch = &a_node->m_branch[index];

      a_stream.WriteArray(curBranch->m_rect.m_min, NUMDIMS);
      a_stream.WriteArray(curBranch->m_rect.m_max, NUMDIMS);

      SaveRec(curBranch->m_child, a_stream);
    }
  }
  else // A leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      Branch* curBranch = &a_node->m_branch[index];

      a_stream.WriteArray(curBranch->m_rect.m_min, NUMDIMS);
      a_stream.WriteArray(curBranch->m_rect.m_max, NUMDIMS);

      a_stream.Write(curBranch->m_data);
    }
  }

  return true; // Should do more error checking on I/O operations
}


RTREE_TEMPLATE
void RTREE_QUAL::RemoveAll()
{
  // Delete all existing nodes
  Reset();

  m_root = AllocNode();
  m_root->m_level = 0;
}


RTREE_TEMPLATE
void RTREE_QUAL::Reset()
{
#ifdef RTREE_DONT_USE_MEMPOOLS
  // Delete all existing nodes
  RemoveAllRec(m_root);
#else // RTREE_DONT_USE_MEMPOOLS
  // Just reset memory pools.  We are not using complex types
  // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


RTREE_TEMPLATE
void RTREE_QUAL::RemoveAllRec(Node* a_node)
{
  ASSERT(a_node);
  ASSERT(a_node->m_level >= 0);

  if(a_node->IsInternalNode()) // This is an internal node in the tree
  {
    for(int index=0; index < a_node->m_count; ++index)
    {
      RemoveAllRec(a_node->m_branch[index].m_child);
    }
  }
  FreeNode(a_node); 
}


RTREE_TEMPLATE
typename RTREE_QUAL::Node* RTREE_QUAL::AllocNode()
{
  Node* newNode;
#ifdef RTREE_DONT_USE_MEMPOOLS
  newNode = new Node;
#else // RTREE_DONT_USE_MEMPOOLS
  // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
  InitNode(newNode);
  return newNode;
}


RTREE_TEMPLATE
void RTREE_QUAL::FreeNode(Node* a_node)
{
  ASSERT(a_node);

#ifdef RTREE_DONT_USE_MEMPOOLS
  delete a_node;
#else // RTREE_DONT_USE_MEMPOOLS
  // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


// Allocate space for a node in the list used in DeletRect to
// store Nodes that are too empty.
RTREE_TEMPLATE
typename RTREE_QUAL::ListNode* RTREE_QUAL::AllocListNode()
{
#ifdef RTREE_DONT_USE_MEMPOOLS
  return new ListNode;
#else // RTREE_DONT_USE_MEMPOOLS
  // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


RTREE_TEMPLATE
void RTREE_QUAL::FreeListNode(ListNode* a_listNode)
{
#ifdef RTREE_DONT_USE_MEMPOOLS
  delete a_listNode;
#else // RTREE_DONT_USE_MEMPOOLS
  // EXAMPLE
#endif // RTREE_DONT_USE_MEMPOOLS
}


RTREE_TEMPLATE
void RTREE_QUAL::InitNode(Node* a_node)
{
  a_node->m_count = 0;
  a_node->m_level = -1;
}


RTREE_TEMPLATE
void RTREE_QUAL::InitRect(Rect* a_rect)
{
  for(int index = 0; index < NUMDIMS; ++index)
  {
    a_rect->m_min[index] = (ELEMTYPE)0;
    a_rect->m_max[index] = (ELEMTYPE)0;
  }
}


// Inserts a new data rectangle into the index structure.
// Recursively descends tree, propagates splits back up.
// Returns 0 if node was not split.  Old node updated.
// If node was split, returns 1 and sets the pointer pointed to by
// new_node to point to the new node.  Old node updated to become one of two.
// The level argument specifies the number of steps up from the leaf
// level to insert; e.g. a data rectangle goes in at level = 0.
RTREE_TEMPLATE
bool RTREE_QUAL::InsertRectRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level)
{
  ASSERT(a_node && a_newNode);
  ASSERT(a_level >= 0 && a_level <= a_node->m_level);

  // recurse until we reach the correct level for the new record. data records
  // will always be called with a_level == 0 (leaf)
  if(a_node->m_level > a_level)
  {
    // Still above level for insertion, go down tree recursively
    Node* otherNode;

    // find the optimal branch for this record
    int index = PickBranch(&a_branch.m_rect, a_node);

    // recursively insert this record into the picked branch
    bool childWasSplit = InsertRectRec(a_branch, a_node->m_branch[index].m_child, &otherNode, a_level);

    if (!childWasSplit)
    {
      // Child was not split. Merge the bounding box of the new record with the
      // existing bounding box
      a_node->m_branch[index].m_rect = CombineRect(&a_branch.m_rect, &(a_node->m_branch[index].m_rect));
      return false;
    }
    else
    {
      // Child was split. The old branches are now re-partitioned to two nodes
      // so we have to re-calculate the bounding boxes of each node
      a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
      Branch branch;
      branch.m_child = otherNode;
      branch.m_rect = NodeCover(otherNode);

      // The old node is already a child of a_node. Now add the newly-created
      // node to a_node as well. a_node might be split because of that.
      return AddBranch(&branch, a_node, a_newNode);
    }
  }
  else if(a_node->m_level == a_level)
  {
    // We have reached level for insertion. Add rect, split if necessary
    return AddBranch(&a_branch, a_node, a_newNode);
  }
  else
  {
    // Should never occur
    ASSERT(0);
    return false;
  }
}


// Insert a data rectangle into an index structure.
// InsertRect provides for splitting the root;
// returns 1 if root was split, 0 if it was not.
// The level argument specifies the number of steps up from the leaf
// level to insert; e.g. a data rectangle goes in at level = 0.
// InsertRect2 does the recursion.
//
RTREE_TEMPLATE
bool RTREE_QUAL::InsertRect(const Branch& a_branch, Node** a_root, int a_level)
{
  ASSERT(a_root);
  ASSERT(a_level >= 0 && a_level <= (*a_root)->m_level);
#ifdef _DEBUG
  for(int index=0; index < NUMDIMS; ++index)
  {
    ASSERT(a_branch.m_rect.m_min[index] <= a_branch.m_rect.m_max[index]);
  }
#endif //_DEBUG  

  Node* newNode;

  if(InsertRectRec(a_branch, *a_root, &newNode, a_level))  // Root split
  {
    // Grow tree taller and new root
    Node* newRoot = AllocNode();
    newRoot->m_level = (*a_root)->m_level + 1;

    Branch branch;

    // add old root node as a child of the new root
    branch.m_rect = NodeCover(*a_root);
    branch.m_child = *a_root;
    AddBranch(&branch, newRoot, NULL);

    // add the split node as a child of the new root
    branch.m_rect = NodeCover(newNode);
    branch.m_child = newNode;
    AddBranch(&branch, newRoot, NULL);

    // set the new root as the root node
    *a_root = newRoot;

    return true;
  }

  return false;
}


// Find the smallest rectangle that includes all rectangles in branches of a node.
RTREE_TEMPLATE
typename RTREE_QUAL::Rect RTREE_QUAL::NodeCover(Node* a_node)
{
  ASSERT(a_node);
  
  Rect rect = a_node->m_branch[0].m_rect;
  for(int index = 1; index < a_node->m_count; ++index)
  {
     rect = CombineRect(&rect, &(a_node->m_branch[index].m_rect));
  }
  
  return rect;
}


// Add a branch to a node.  Split the node if necessary.
// Returns 0 if node not split.  Old node updated.
// Returns 1 if node split, sets *new_node to address of new node.
// Old node updated, becomes one of two.
RTREE_TEMPLATE
bool RTREE_QUAL::AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode)
{
  ASSERT(a_branch);
  ASSERT(a_node);

  if(a_node->m_count < MAXNODES)  // Split won't be necessary
  {
    a_node->m_branch[a_node->m_count] = *a_branch;
    ++a_node->m_count;

    return false;
  }
  else
  {
    ASSERT(a_newNode);
    
    SplitNode(a_node, a_branch, a_newNode);
    return true;
  }
}


// Disconnect a dependent node.
// Caller must return (or stop using iteration index) after this as count has changed
RTREE_TEMPLATE
void RTREE_QUAL::DisconnectBranch(Node* a_node, int a_index)
{
  ASSERT(a_node && (a_index >= 0) && (a_index < MAXNODES));
  ASSERT(a_node->m_count > 0);

  // Remove element by swapping with the last element to prevent gaps in array
  a_node->m_branch[a_index] = a_node->m_branch[a_node->m_count - 1];
  
  --a_node->m_count;
}


// Pick a branch.  Pick the one that will need the smallest increase
// in area to accomodate the new rectangle.  This will result in the
// least total area for the covering rectangles in the current node.
// In case of a tie, pick the one which was smaller before, to get
// the best resolution when searching.
RTREE_TEMPLATE
int RTREE_QUAL::PickBranch(const Rect* a_rect, Node* a_node)
{
  ASSERT(a_rect && a_node);
  
  bool firstTime = true;
  ELEMTYPEREAL increase;
  ELEMTYPEREAL bestIncr = (ELEMTYPEREAL)-1;
  ELEMTYPEREAL area;
  ELEMTYPEREAL bestArea;
  int best = 0;
  Rect tempRect;

  for(int index=0; index < a_node->m_count; ++index)
  {
    Rect* curRect = &a_node->m_branch[index].m_rect;
    area = CalcRectVolume(curRect);
    tempRect = CombineRect(a_rect, curRect);
    increase = CalcRectVolume(&tempRect) - area;
    if((increase < bestIncr) || firstTime)
    {
      best = index;
      bestArea = area;
      bestIncr = increase;
      firstTime = false;
    }
    else if((increase == bestIncr) && (area < bestArea))
    {
      best = index;
      bestArea = area;
      bestIncr = increase;
    }
  }
  return best;
}


// Combine two rectangles into larger one containing both
RTREE_TEMPLATE
typename RTREE_QUAL::Rect RTREE_QUAL::CombineRect(const Rect* a_rectA, const Rect* a_rectB)
{
  ASSERT(a_rectA && a_rectB);

  Rect newRect;

  for(int index = 0; index < NUMDIMS; ++index)
  {
    newRect.m_min[index] = Min(a_rectA->m_min[index], a_rectB->m_min[index]);
    newRect.m_max[index] = Max(a_rectA->m_max[index], a_rectB->m_max[index]);
  }

  return newRect;
}



// Split a node.
// Divides the nodes branches and the extra one between two nodes.
// Old node is one of the new ones, and one really new one is created.
// Tries more than one method for choosing a partition, uses best result.
RTREE_TEMPLATE
void RTREE_QUAL::SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode)
{
  ASSERT(a_node);
  ASSERT(a_branch);

  // Could just use local here, but member or external is faster since it is reused
  PartitionVars localVars;
  PartitionVars* parVars = &localVars;

  // Load all the branches into a buffer, initialize old node
  GetBranches(a_node, a_branch, parVars);

  // Find partition
  ChoosePartition(parVars, MINNODES);

  // Create a new node to hold (about) half of the branches
  *a_newNode = AllocNode();
  (*a_newNode)->m_level = a_node->m_level;

  // Put branches from buffer into 2 nodes according to the chosen partition
  a_node->m_count = 0;
  LoadNodes(a_node, *a_newNode, parVars);
  
  ASSERT((a_node->m_count + (*a_newNode)->m_count) == parVars->m_total);
}


// Calculate the n-dimensional volume of a rectangle
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::RectVolume(Rect* a_rect)
{
  ASSERT(a_rect);
  
  ELEMTYPEREAL volume = (ELEMTYPEREAL)1;

  for(int index=0; index<NUMDIMS; ++index)
  {
    volume *= a_rect->m_max[index] - a_rect->m_min[index];
  }
  
  ASSERT(volume >= (ELEMTYPEREAL)0);
  
  return volume;
}


// The exact volume of the bounding sphere for the given Rect
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::RectSphericalVolume(Rect* a_rect)
{
  ASSERT(a_rect);
   
  ELEMTYPEREAL sumOfSquares = (ELEMTYPEREAL)0;
  ELEMTYPEREAL radius;

  for(int index=0; index < NUMDIMS; ++index) 
  {
    ELEMTYPEREAL halfExtent = ((ELEMTYPEREAL)a_rect->m_max[index] - (ELEMTYPEREAL)a_rect->m_min[index]) * 0.5f;
    sumOfSquares += halfExtent * halfExtent;
  }

  radius = (ELEMTYPEREAL)sqrt(sumOfSquares);
  
  // Pow maybe slow, so test for common dims like 2,3 and just use x*x, x*x*x.
  if(NUMDIMS == 3)
  {
    return (radius * radius * radius * m_unitSphereVolume);
  }
  else if(NUMDIMS == 2)
  {
    return (radius * radius * m_unitSphereVolume);
  }
  else
  {
    return (ELEMTYPEREAL)(pow(radius, NUMDIMS) * m_unitSphereVolume);
  }
}


// Use one of the methods to calculate retangle volume
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::CalcRectVolume(Rect* a_rect)
{
#ifdef RTREE_USE_SPHERICAL_VOLUME
  return RectSphericalVolume(a_rect); // Slower but helps certain merge cases
#else // RTREE_USE_SPHERICAL_VOLUME
  return RectVolume(a_rect); // Faster but can cause poor merges
#endif // RTREE_USE_SPHERICAL_VOLUME  
}


// Load branch buffer with branches from full node plus the extra branch.
RTREE_TEMPLATE
void RTREE_QUAL::GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars)
{
  ASSERT(a_node);
  ASSERT(a_branch);

  ASSERT(a_node->m_count == MAXNODES);
    
  // Load the branch buffer
  for(int index=0; index < MAXNODES; ++index)
  {
    a_parVars->m_branchBuf[index] = a_node->m_branch[index];
  }
  a_parVars->m_branchBuf[MAXNODES] = *a_branch;
  a_parVars->m_branchCount = MAXNODES + 1;

  // Calculate rect containing all in the set
  a_parVars->m_coverSplit = a_parVars->m_branchBuf[0].m_rect;
  for(int index=1; index < MAXNODES+1; ++index)
  {
    a_parVars->m_coverSplit = CombineRect(&a_parVars->m_coverSplit, &a_parVars->m_branchBuf[index].m_rect);
  }
  a_parVars->m_coverSplitArea = CalcRectVolume(&a_parVars->m_coverSplit);
}


// Method #0 for choosing a partition:
// As the seeds for the two groups, pick the two rects that would waste the
// most area if covered by a single rectangle, i.e. evidently the worst pair
// to have in the same group.
// Of the remaining, one at a time is chosen to be put in one of the two groups.
// The one chosen is the one with the greatest difference in area expansion
// depending on which group - the rect most strongly attracted to one group
// and repelled from the other.
// If one group gets too full (more would force other group to violate min
// fill requirement) then other group gets the rest.
// These last are the ones that can go in either group most easily.
RTREE_TEMPLATE
void RTREE_QUAL::ChoosePartition(PartitionVars* a_parVars, int a_minFill)
{
  ASSERT(a_parVars);
  
  ELEMTYPEREAL biggestDiff;
  int group, chosen = 0, betterGroup = 0;
  
  InitParVars(a_parVars, a_parVars->m_branchCount, a_minFill);
  PickSeeds(a_parVars);

  while (((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
       && (a_parVars->m_count[0] < (a_parVars->m_total - a_parVars->m_minFill))
       && (a_parVars->m_count[1] < (a_parVars->m_total - a_parVars->m_minFill)))
  {
    biggestDiff = (ELEMTYPEREAL) -1;
    for(int index=0; index<a_parVars->m_total; ++index)
    {
      if(PartitionVars::NOT_TAKEN == a_parVars->m_partition[index])
      {
        Rect* curRect = &a_parVars->m_branchBuf[index].m_rect;
        Rect rect0 = CombineRect(curRect, &a_parVars->m_cover[0]);
        Rect rect1 = CombineRect(curRect, &a_parVars->m_cover[1]);
        ELEMTYPEREAL growth0 = CalcRectVolume(&rect0) - a_parVars->m_area[0];
        ELEMTYPEREAL growth1 = CalcRectVolume(&rect1) - a_parVars->m_area[1];
        ELEMTYPEREAL diff = growth1 - growth0;
        if(diff >= 0)
        {
          group = 0;
        }
        else
        {
          group = 1;
          diff = -diff;
        }

        if(diff > biggestDiff)
        {
          biggestDiff = diff;
          chosen = index;
          betterGroup = group;
        }
        else if((diff == biggestDiff) && (a_parVars->m_count[group] < a_parVars->m_count[betterGroup]))
        {
          chosen = index;
          betterGroup = group;
        }
      }
    }
    Classify(chosen, betterGroup, a_parVars);
  }

  // If one group too full, put remaining rects in the other
  if((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
  {
    if(a_parVars->m_count[0] >= a_parVars->m_total - a_parVars->m_minFill)
    {
      group = 1;
    }
    else
    {
      group = 0;
    }
    for(int index=0; index<a_parVars->m_total; ++index)
    {
      if(PartitionVars::NOT_TAKEN == a_parVars->m_partition[index])
      {
        Classify(index, group, a_parVars);
      }
    }
  }

  ASSERT((a_parVars->m_count[0] + a_parVars->m_count[1]) == a_parVars->m_total);
  ASSERT((a_parVars->m_count[0] >= a_parVars->m_minFill) && 
        (a_parVars->m_count[1] >= a_parVars->m_minFill));
}


// Copy branches from the buffer into two nodes according to the partition.
RTREE_TEMPLATE
void RTREE_QUAL::LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars)
{
  ASSERT(a_nodeA);
  ASSERT(a_nodeB);
  ASSERT(a_parVars);

  for(int index=0; index < a_parVars->m_total; ++index)
  {
    ASSERT(a_parVars->m_partition[index] == 0 || a_parVars->m_partition[index] == 1);

    int targetNodeIndex = a_parVars->m_partition[index];
    Node* targetNodes[] = {a_nodeA, a_nodeB};

    // It is assured that AddBranch here will not cause a node split. 
    bool nodeWasSplit = AddBranch(&a_parVars->m_branchBuf[index], targetNodes[targetNodeIndex], NULL);
    ASSERT(!nodeWasSplit);
  }
}


// Initialize a PartitionVars structure.
RTREE_TEMPLATE
void RTREE_QUAL::InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill)
{
  ASSERT(a_parVars);

  a_parVars->m_count[0] = a_parVars->m_count[1] = 0;
  a_parVars->m_area[0] = a_parVars->m_area[1] = (ELEMTYPEREAL)0;
  a_parVars->m_total = a_maxRects;
  a_parVars->m_minFill = a_minFill;
  for(int index=0; index < a_maxRects; ++index)
  {
    a_parVars->m_partition[index] = PartitionVars::NOT_TAKEN;
  }
}


RTREE_TEMPLATE
void RTREE_QUAL::PickSeeds(PartitionVars* a_parVars)
{
  int seed0 = 0, seed1 = 0;
  ELEMTYPEREAL worst, waste;
  ELEMTYPEREAL area[MAXNODES+1];

  for(int index=0; index<a_parVars->m_total; ++index)
  {
    area[index] = CalcRectVolume(&a_parVars->m_branchBuf[index].m_rect);
  }

  worst = -a_parVars->m_coverSplitArea - 1;
  for(int indexA=0; indexA < a_parVars->m_total-1; ++indexA)
  {
    for(int indexB = indexA+1; indexB < a_parVars->m_total; ++indexB)
    {
      Rect oneRect = CombineRect(&a_parVars->m_branchBuf[indexA].m_rect, &a_parVars->m_branchBuf[indexB].m_rect);
      waste = CalcRectVolume(&oneRect) - area[indexA] - area[indexB];
      if(waste > worst)
      {
        worst = waste;
        seed0 = indexA;
        seed1 = indexB;
      }
    }
  }

  Classify(seed0, 0, a_parVars);
  Classify(seed1, 1, a_parVars);
}


// Put a branch in one of the groups.
RTREE_TEMPLATE
void RTREE_QUAL::Classify(int a_index, int a_group, PartitionVars* a_parVars)
{
  ASSERT(a_parVars);
  ASSERT(PartitionVars::NOT_TAKEN == a_parVars->m_partition[a_index]);

  a_parVars->m_partition[a_index] = a_group;

  // Calculate combined rect
  if (a_parVars->m_count[a_group] == 0)
  {
    a_parVars->m_cover[a_group] = a_parVars->m_branchBuf[a_index].m_rect;
  }
  else
  {
    a_parVars->m_cover[a_group] = CombineRect(&a_parVars->m_branchBuf[a_index].m_rect, &a_parVars->m_cover[a_group]);
  }

  // Calculate volume of combined rect
  a_parVars->m_area[a_group] = CalcRectVolume(&a_parVars->m_cover[a_group]);

  ++a_parVars->m_count[a_group];
}


// Delete a data rectangle from an index structure.
// Pass in a pointer to a Rect, the tid of the record, ptr to ptr to root node.
// Returns 1 if record not found, 0 if success.
// RemoveRect provides for eliminating the root.
RTREE_TEMPLATE
bool RTREE_QUAL::RemoveRect(Rect* a_rect, const DATATYPE& a_id, Node** a_root)
{
  ASSERT(a_rect && a_root);
  ASSERT(*a_root);

  ListNode* reInsertList = NULL;

  if(!RemoveRectRec(a_rect, a_id, *a_root, &reInsertList))
  {
    // Found and deleted a data item
    // Reinsert any branches from eliminated nodes
    while(reInsertList)
    {
      Node* tempNode = reInsertList->m_node;

      for(int index = 0; index < tempNode->m_count; ++index)
      {
        // TODO go over this code. should I use (tempNode->m_level - 1)?
        InsertRect(tempNode->m_branch[index],
                   a_root,
                   tempNode->m_level);
      }
      
      ListNode* remLNode = reInsertList;
      reInsertList = reInsertList->m_next;
      
      FreeNode(remLNode->m_node);
      FreeListNode(remLNode);
    }
    
    // Check for redundant root (not leaf, 1 child) and eliminate TODO replace
    // if with while? In case there is a whole branch of redundant roots...
    if((*a_root)->m_count == 1 && (*a_root)->IsInternalNode())
    {
      Node* tempNode = (*a_root)->m_branch[0].m_child;
      
      ASSERT(tempNode);
      FreeNode(*a_root);
      *a_root = tempNode;
    }
    return false;
  }
  else
  {
    return true;
  }
}


// Delete a rectangle from non-root part of an index structure.
// Called by RemoveRect.  Descends tree recursively,
// merges branches on the way back up.
// Returns 1 if record not found, 0 if success.
RTREE_TEMPLATE
bool RTREE_QUAL::RemoveRectRec(Rect* a_rect, const DATATYPE& a_id, Node* a_node, ListNode** a_listNode)
{
  ASSERT(a_rect && a_node && a_listNode);
  ASSERT(a_node->m_level >= 0);

  if(a_node->IsInternalNode())  // not a leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      if(Overlap(a_rect, &(a_node->m_branch[index].m_rect)))
      {
        if(!RemoveRectRec(a_rect, a_id, a_node->m_branch[index].m_child, a_listNode))
        {
          if(a_node->m_branch[index].m_child->m_count >= MINNODES)
          {
            // child removed, just resize parent rect
            a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
          }
          else
          {
            // child removed, not enough entries in node, eliminate node
            ReInsert(a_node->m_branch[index].m_child, a_listNode);
            DisconnectBranch(a_node, index); // Must return after this call as count has changed
          }
          return false;
        }
      }
    }
    return true;
  }
  else // A leaf node
  {
    for(int index = 0; index < a_node->m_count; ++index)
    {
      if(a_node->m_branch[index].m_data == a_id)
      {
        DisconnectBranch(a_node, index); // Must return after this call as count has changed
        return false;
      }
    }
    return true;
  }
}


// Decide whether two rectangles overlap.
RTREE_TEMPLATE
bool RTREE_QUAL::Overlap(const Rect* a_rectA, const Rect* a_rectB) const
{
  ASSERT(a_rectA && a_rectB);

  for(int index=0; index < NUMDIMS; ++index)
  {
    if (a_rectA->m_min[index] > a_rectB->m_max[index] ||
        a_rectB->m_min[index] > a_rectA->m_max[index])
    {
      return false;
    }
  }
  return true;
}


// Add a node to the reinsertion list.  All its branches will later
// be reinserted into the index structure.
RTREE_TEMPLATE
void RTREE_QUAL::ReInsert(Node* a_node, ListNode** a_listNode)
{
  ListNode* newListNode;

  newListNode = AllocListNode();
  newListNode->m_node = a_node;
  newListNode->m_next = *a_listNode;
  *a_listNode = newListNode;
}


// Search in an index tree or subtree for all data retangles that overlap the argument rectangle.
// return true if it contains at least one result, false otherwise
RTREE_TEMPLATE
bool RTREE_QUAL::YoNSearch(Node* a_node, Rect* a_rect) const
{
  ASSERT(a_node);
  ASSERT(a_node->m_level >= 0);
  ASSERT(a_rect);

  if(a_node->IsInternalNode())
  {
    // This is an internal node in the tree
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(a_node->m_branch[index].m_rect.m_min[0] > a_rect->m_max[0])
      {
        break;
      }
      if(Overlap(a_rect, &a_node->m_branch[index].m_rect))
      {
        if(YoNSearch(a_node->m_branch[index].m_child, a_rect))
        {
          return true;
        }
      }
    }
  }
  else
  {
    // This is a leaf node
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(a_node->m_branch[index].m_rect.m_min[0] > a_rect->m_max[0])
      {
        break;
      }
      if(Overlap(a_rect, &a_node->m_branch[index].m_rect))
      {
        return true;
      }
    }
  }

  return false; // Not found
}

// Search in an index tree or subtree for all data retangles that overlap the argument rectangle.
RTREE_TEMPLATE
bool RTREE_QUAL::Search(Node* a_node, Rect* a_rect, int& a_foundCount, std::vector<DATATYPE>& ret) const
{
  ASSERT(a_node);
  ASSERT(a_node->m_level >= 0);
  ASSERT(a_rect);

  if(a_node->IsInternalNode())
  {
    // This is an internal node in the tree
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(a_node->m_branch[index].m_rect.m_min[0] > a_rect->m_max[0])
      {
        break;
      }
      if(Overlap(a_rect, &a_node->m_branch[index].m_rect))
      {
        if(!Search(a_node->m_branch[index].m_child, a_rect, a_foundCount, ret))
        {
          // The callback indicated to stop searching
          return false;
        }
      }
    }
  }
  else
  {
    // This is a leaf node
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(a_node->m_branch[index].m_rect.m_min[0] > a_rect->m_max[0])
      {
        break;
      }
      if(Overlap(a_rect, &a_node->m_branch[index].m_rect))
      {
        DATATYPE id = a_node->m_branch[index].m_data;
        ++a_foundCount;
        ret.push_back(id);
      }
    }
  }

  return true; // Continue searching
}

RTREE_TEMPLATE
bool RTREE_QUAL::NodeSearch(Node* a_node, Rect* a_rect, int& a_foundCount, std::vector<Node*>& ret, int stop_level) const
{
  ASSERT(stop_level > 0);
  ASSERT(a_node);
  ASSERT(a_node->m_level > stop_level);
  ASSERT(a_rect);

  bool to_return = (a_node->m_level - stop_level == 1);

  for(int index=0; index < a_node->m_count; ++index)
  {
    if(Overlap(a_rect, &a_node->m_branch[index].m_rect))
    {
      if(to_return)
      {
        ++a_foundCount;
        ret.push_back(a_node->m_branch[index].m_child);
      }
      else
      {
        NodeSearch(a_node->m_branch[index].m_child, a_rect, a_foundCount, ret, stop_level);
      }
    }
  }

  return true;

}

// RTREE_TEMPLATE
// bool RTREE_QUAL::CompBranch(const Branch& a, const Branch& b) {
//   return (a.m_rect.m_min[0] < b.m_rect.m_min[0]);
// }

RTREE_TEMPLATE
void RTREE_QUAL::SortDim0Node(Node* a_node)
{
  ASSERT(a_node);

  // std::sort(std::begin(a_node->m_branch), std::begin(a_node->m_branch) + a_node->m_count, [this] (Branch a, Branch b) { return CompBranch(a, b); });
  std::sort(std::begin(a_node->m_branch), std::begin(a_node->m_branch) + a_node->m_count, [this] (Branch a, Branch b) { return (a.m_rect.m_min[0] < b.m_rect.m_min[0]); });

  if (a_node->IsInternalNode()) {
    for (int i = 0; i < a_node->m_count; i++) {
      SortDim0Node(a_node->m_branch[i].m_child);
    }
  }
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
bool RTREE_QUAL::Overlap(const Rect* rect, const typename RTREE_QUAL_PRIME::Rect* rect_prime) const
{
  for (int index = 0; index < NUMDIMS; index++)
  {
    if (rect->m_min[index] > rect_prime->m_max[index] || rect_prime->m_min[index] > rect->m_max[index])
    {
        return false;
    }
  }
  return true;
}

RTREE_TEMPLATE
bool RTREE_QUAL::Overlap(const Rect* rect, const OUTRA_RECT* ovlp_rect) const
{
  for (int index = 0; index < NUMDIMS; index++)
  {
    if (rect->m_min[index] > ovlp_rect->m_max[index] || ovlp_rect->m_min[index] > rect->m_max[index])
    {
      return false;
    }
  }
  return true;
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
void RTREE_QUAL::OverlappingArea(const Rect* rect, const typename RTREE_QUAL_PRIME::Rect* rect_prime, OUTRA_RECT& ret) const
{
  for (int index = 0; index < NUMDIMS; index++)
  {
    ret.m_min[index] = std::max(rect->m_min[index], rect_prime->m_min[index]);
    ret.m_max[index] = std::min(rect->m_max[index], rect_prime->m_max[index]);
  }
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::ResidualSearch(const Node* node, const typename RTREE_QUAL_PRIME::Rect* rect_prime, const OUTRA_RECT* ovlp_rect, std::vector<int>& ret) const
{
	for (int i = 0; i < node->m_count; i++)
	{
		auto br = node->m_branch[i];
		auto rect = br.m_rect;

		if (!Overlap(&rect, ovlp_rect))
		{
			continue;
		}

		if (!OVERLAP_P(&rect, rect_prime))
		{
			continue;
		}

		if (node->IsLeaf()) {
			ret.push_back(br.m_data);
		} else {
    	OUTRA_RECT new_ovlp_rect;
      OVERLAPPING_AREA_P(&rect, rect_prime, new_ovlp_rect);

			RESIDUAL_SEARCH_P(br.m_child, rect_prime, &new_ovlp_rect, ret);
		}

	}
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::JoinNode(const Node* node, const RTREE_QUAL_PRIME* tree_prime, const typename RTREE_QUAL_PRIME::Node* node_prime, const OUTRA_RECT* ovlp_rect, std::vector<std::pair<int, int>>& ret) const
{
  for (int i = 0; i < node->m_count; i++)
  {
    auto br = node->m_branch[i];
    auto rect = br.m_rect;

    if (!Overlap(&rect, ovlp_rect))
    {
      continue;
    }

    for (int j = 0; j < node_prime->m_count; j++)
    {
      auto br_prime = node_prime->m_branch[j];
      auto rect_prime = br_prime.m_rect;

      if (!tree_prime->Overlap(&rect_prime, ovlp_rect))
      {
        continue;
      }

      if (!OVERLAP_P(&rect, &rect_prime))
      {
        continue;
      }

      if (node->IsLeaf() && node_prime->IsLeaf())
      {
        ret.push_back(std::make_pair(br_prime.m_data, br.m_data)); // stop search when both index and aux reach leaf
      }
      else
      {
      	OUTRA_RECT new_ovlp_rect;
        OVERLAPPING_AREA_P(&rect, &rect_prime, new_ovlp_rect);

      	if (node->IsInternalNode() && node_prime->IsLeaf())
	      {
          std::vector<int> residual_ret;
	        RESIDUAL_SEARCH_P(br.m_child, &rect_prime, &new_ovlp_rect, residual_ret); // continue search residual subtree of index node
          for (int &r: residual_ret)
          {
            ret.push_back(std::make_pair(br_prime.m_data, r));
          }
	      }
	      else if (node->IsLeaf() && node_prime->IsInternalNode())
	      {
          std::vector<int> residual_ret;
	        tree_prime->RESIDUAL_SEARCH(br_prime.m_child, &rect, &new_ovlp_rect, residual_ret); // continue search residual subtree of aux node
          for (int &r: residual_ret)
          {
            ret.push_back(std::make_pair(r, br.m_data));
          }
	      }
	      else
	      {
	        JOIN_NODE_P(br.m_child, tree_prime, br_prime.m_child, &new_ovlp_rect, ret);
	      }
    	}
    }
  }
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::SortedResidualSearch(const Node* node, const typename RTREE_QUAL_PRIME::Rect* rect_prime, const OUTRA_RECT* ovlp_rect, std::vector<int>& ret) const
{
	for (int i = 0; i < node->m_count; i++)
	{
		auto br = node->m_branch[i];
		auto rect = br.m_rect;

		if (rect.m_min[0] > rect_prime->m_max[0])
		{
			break;
		}

		if (!Overlap(&rect, ovlp_rect))
		{
			continue;
		}

		if (!OVERLAP_P(&rect, rect_prime))
		{
			continue;
		}

		if (node->IsLeaf()) {
			ret.push_back(br.m_data);
		} else {
    	OUTRA_RECT new_ovlp_rect;
      OVERLAPPING_AREA_P(&rect, rect_prime, new_ovlp_rect);

			SORTED_RESIDUAL_SEARCH_P(br.m_child, rect_prime, &new_ovlp_rect, ret);
		}
	}
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::JoinInternalLoop(const Rect* rect, int unmarked, const typename RTREE_QUAL_PRIME::Node* node_prime, std::vector<int>& ret) const
{
  int k = unmarked;

  while (k < node_prime->m_count && node_prime->m_branch[k].m_rect.m_min[0] <= rect->m_max[0])
  {
    bool intersect = true;
    for (int index = 1; index < NUMDIMS; index++)
    {
      if (rect->m_min[index] > node_prime->m_branch[k].m_rect.m_max[index] || node_prime->m_branch[k].m_rect.m_min[index] > rect->m_max[index])
      {
        intersect = false;
        break;
      }
    }
    if (intersect)
    {
    	ret.push_back(k);
    }
    k++;
  }
}

RTREE_TEMPLATE
RTREE_TEMPLATE_PRIME
int RTREE_QUAL::JoinSortedNode(const Node* node, const RTREE_QUAL_PRIME* tree_prime, const typename RTREE_QUAL_PRIME::Node* node_prime, const OUTRA_RECT* ovlp_rect, std::vector<std::pair<int, int>>& ret) const
{
  int i = 0, j = 0;
  std::vector<std::pair<int, int>> inters_pairs;

  while ((i < node->m_count) && (j < node_prime->m_count) && (node->m_branch[i].m_rect.m_min[0] <= ovlp_rect->m_max[0]) && (node_prime->m_branch[j].m_rect.m_min[0] <= ovlp_rect->m_max[0]))
  {
    if (node->m_branch[i].m_rect.m_min[0] < node_prime->m_branch[j].m_rect.m_min[0])
    {
    	if (Overlap(&(node->m_branch[i].m_rect), ovlp_rect))
    	{
    		std::vector<int> ret_a;
      	JOIN_INT_NODE_P(&(node->m_branch[i].m_rect), j, node_prime, ret_a);
      	for (int &a: ret_a)
      	{
      		inters_pairs.push_back(std::make_pair(i, a));
      	}
      }
      i++;
    }
    else
    {
    	if (tree_prime->Overlap(&(node_prime->m_branch[j].m_rect), ovlp_rect))
    	{
    		std::vector<int> ret_b;
    		tree_prime->JOIN_INT_NODE(&(node_prime->m_branch[j].m_rect), i, node, ret_b);
    		for (int &b: ret_b)
    		{
    			inters_pairs.push_back(std::make_pair(b, j));
    		}
    	}
    	j++;
    }
  }


  for (auto &p: inters_pairs)
  {
    auto br = node->m_branch[p.first];
    auto br_prime = node_prime->m_branch[p.second];

    if (node->IsLeaf() && node_prime->IsLeaf())
    {
    	ret.push_back(std::make_pair(br_prime.m_data, br.m_data)); // stop search when both index and aux reach leaf
    }
    else
    {
	  	OUTRA_RECT new_ovlp_rect;
	    OVERLAPPING_AREA_P(&(br.m_rect), &(br_prime.m_rect), new_ovlp_rect);

	    if (node->IsInternalNode() && node_prime->IsLeaf())
	    {
        std::vector<int> residual_ret;
	     	SORTED_RESIDUAL_SEARCH_P(br.m_child, &(br_prime.m_rect), &new_ovlp_rect, residual_ret); // continue search residual subtree of index node
        for (int &r: residual_ret)
        {
          ret.push_back(std::make_pair(br_prime.m_data, r));
        }
	    }
	    else if (node->IsLeaf() && node_prime->IsInternalNode())
	    {
        std::vector<int> residual_ret;
	  	  tree_prime->SORTED_RESIDUAL_SEARCH(br_prime.m_child, &(br.m_rect), &new_ovlp_rect, residual_ret); // continue search residual subtree of aux node
        for (int &r: residual_ret)
        {
          ret.push_back(std::make_pair(r, br.m_data));
        }
	    }
	    else
	    {
		    JOIN_SORTED_NODE_P(br.m_child, tree_prime, br_prime.m_child, &new_ovlp_rect, ret);
	  	}
  	}
  }
}

RTREE_TEMPLATE
int RTREE_QUAL::SphereSearchNode(const Node* a_node, const ELEMTYPE center[NUMDIMS], ELEMTYPEREAL sq_min, ELEMTYPEREAL sq_max, std::vector<DATATYPE>& ret) const
{
  ASSERT(a_node);
  ASSERT(a_node->m_level >= 0);

  if(a_node->IsInternalNode())
  {
    // This is an internal node in the tree
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(SqMinDist(center, &(a_node->m_branch[index].m_rect)) <= sq_max && SqMaxDist(center, &(a_node->m_branch[index].m_rect)) >= sq_min)
      {
        SphereSearchNode(a_node->m_branch[index].m_child, center, sq_min, sq_max, ret);
      }
    }
  }
  else
  {
    // This is a leaf node
    for(int index=0; index < a_node->m_count; ++index)
    {
      auto sq = SqDist(center, &(a_node->m_branch[index].m_rect));
      if(sq_min <= sq && sq <= sq_max)
      {
        ret.push_back(a_node->m_branch[index].m_data);
      }
    }
  }

  return 0;

}

RTREE_TEMPLATE
void RTREE_QUAL::knnVertexSearchNode(const Node* a_node, const ELEMTYPE center[NUMDIMS], int k, std::vector<DATATYPE>& ret, std::vector<ELEMTYPEREAL>& sq_dist_arr) const
{
  ASSERT(a_node);
  ASSERT(a_node->m_level >= 0);

  if(a_node->IsInternalNode())
  {
    // This is an internal node in the tree
    for(int index=0; index < a_node->m_count; ++index)
    {
      if(SqMinDist(center, &(a_node->m_branch[index].m_rect)) <= sq_dist_arr[k - 1])
      {
        knnVertexSearchNode(a_node->m_branch[index].m_child, center, k, ret, sq_dist_arr);
      }
    }
  }
  else
  {
    // This is a leaf node
    for(int index=0; index < a_node->m_count; ++index)
    {
      if (SqMinDist(center, &(a_node->m_branch[index].m_rect)) <= sq_dist_arr[k - 1])
      {
        ELEMTYPEREAL new_dist = SqDist(center, &(a_node->m_branch[index]));
        if (new_dist < sq_dist_arr[k - 1]) {
          for (int i = 0; i < k; i++) { // from 0 to (k-1): e.g., k = 5, the current dist array is: [100, 200, 240, 300, 400]
            if (new_dist < sq_dist_arr[i]) { // search for the first dist larger than new_dist, then insert before it: e.g., new_dist = 220, then insert before i = 2
              auto subst_dist = new_dist; // set initial substition to new_dist
              auto subst_data = a_node->m_branch[index].m_data;
              for (int j = i; j < k; j++) { // from i = 2 to 4
                auto tmp_dist = sq_dist_arr[j];
                auto tmp_data = ret[j];
                sq_dist_arr[j] = subst_dist;
                ret[j] = subst_data;
                subst_dist = tmp_dist;
                subst_data = tmp_data;
              }
            }
            break;
          }
        }
      }
    }
  }

}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqDist(const ELEMTYPE center[NUMDIMS], const Branch* a_branch) const
{
  if (a_branch->m_is_tri) {
    return SqDistPtTri(center, &(a_branch->m_tri));
  } else {
    return SqDist(center, &(a_branch->m_rect));
  }
}

// This calculates the distance when the leaf is a point
RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const
{
  ELEMTYPEREAL sq = 0;
  for (int i = 0; i < NUMDIMS; i++)
  {
    ELEMTYPE diff = center[i] - a_rect->m_min[i];
    sq += ((ELEMTYPEREAL) diff) * ((ELEMTYPEREAL) diff);
  }
  return sq;
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqDist(const ELEMTYPE p[NUMDIMS], const ELEMTYPE q[NUMDIMS]) const
{
  ELEMTYPEREAL sq = 0;
  for (int i = 0; i < NUMDIMS; i++)
  {
    ELEMTYPE diff = p[i] - q[i];
    sq += ((ELEMTYPEREAL) diff) * ((ELEMTYPEREAL) diff);
  }
  return sq;
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqMinDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const
{
  ELEMTYPEREAL sq = 0;
  
  for(int i = 0; i < NUMDIMS; i++)
  {
    if (center[i] > a_rect->m_max[i])
    {
      ELEMTYPE diff = center[i] - a_rect->m_max[i];
      sq += diff * diff;
    }
    else if (center[i] < a_rect->m_min[i])
    {
      ELEMTYPE diff = a_rect->m_min[i] - center[i];
      sq += diff * diff;
    }
  }
  
  return sq;
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqMaxDist(const ELEMTYPE center[NUMDIMS], const Rect* a_rect) const
{
  ELEMTYPEREAL sq = 0;

  for (int i = 0; i < NUMDIMS; i++) {
    ELEMTYPE diff = (center[i] * 2 < a_rect->m_min[i] + a_rect->m_max[i]) ? (a_rect->m_max[i] - center[i]) : (center[i] - a_rect->m_min[i]);
    sq += diff * diff;
  }

  return sq;
}

inline void vec_neg_int(const long long a[3], long long x[3])
{
  for (int i = 0; i < 3; i++) {
    x[i] = -a[3];
  }
}

inline void vec_minus_int(const int a[3], const int b[3], long long x[3])
{
  for (int i = 0; i < 3; i++) {
    x[i] = -a[3];
  }
}

inline void vec_minus_int(const long long a[3], const long long b[3], long long x[3])
{
  for (int i = 0; i < 3; i++) {
    x[i] = -a[3];
  }
}

inline long long vec_sq_norm_int(const int a[3])
{
  long long ret = 0;
  for (int i = 0; i < 3; i++) {
    ret += a[i] * a[i];
  }
  return ret;
}

inline long long vec_sq_norm_int(const long long a[3])
{
  long long ret = 0;
  for (int i = 0; i < 3; i++) {
    ret += a[i] * a[i];
  }
  return ret;
}

inline void vec_cross_int(const long long a[3], const long long b[3], long long x[3])
{
  x[0] = a[1] * b[2] - a[2] * b[1];
  x[1] = a[2] * b[0] - a[0] * b[2];
  x[2] = a[0] * b[1] - a[1] * b[0];
}

inline long long vec_dot_int(const long long a[3], const long long b[3])
{
  return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

inline long long determ(const long long A[3][3])
{
  return A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) +
         A[0][1] * (A[1][2] * A[2][0] - A[1][0] * A[2][2]) +
         A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
}

// Function to get adjoint of A[N][N] in adj[N][N].
inline void adjoint(const long long A[3][3], long long adj[3][3])
{
  adj[0][0] = A[1][1] * A[2][2] - A[1][2] * A[2][1];
  adj[0][1] = A[0][2] * A[2][1] - A[0][1] * A[2][2];
  adj[0][2] = A[0][1] * A[1][2] - A[0][2] * A[1][1];

  adj[1][0] = A[2][0] * A[1][2] - A[1][0] * A[2][2];
  adj[1][1] = A[0][0] * A[2][2] - A[0][2] * A[2][0];
  adj[1][2] = A[0][2] * A[1][0] - A[0][0] * A[1][2];

  adj[2][0] = A[1][0] * A[2][1] - A[1][1] * A[2][0];
  adj[2][1] = A[0][1] * A[2][0] - A[0][0] * A[2][1];
  adj[2][2] = A[0][0] * A[1][1] - A[0][1] * A[1][0];
}

bool inverse(const long long a[3], const long long b[3], const long long c[3], double x[3][3])
{
  long long A[3][3];
  for (int i = 0; i < 3; i++) {
    A[i][0] = a[i];
    A[i][1] = b[i];
    A[i][2] = c[i];
  }

  double det_A = determ(A);
  if (det_A == 0) {
    cout << "Singular 3x3 matrix, can't find its inverse" << endl;
    return false;
  }

  // Find adjoint
  long long adj[3][3];
  adjoint(A, adj);

  // Get inverse
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      x[i][j] = double(adj[i][j]) / det_A;
    }
  }

  return true;
}

RTREE_TEMPLATE
bool RTREE_QUAL::CheckPtProjInsideTri(const ELEMTYPE q[NUMDIMS], const Tri* a_tri, ELEMTYPE proj[NUMDIMS]) const
{

  // point ab = fv[1] - fv[0];
  // point ac = fv[2] - fv[0];
  // point cr = ab CROSS ac;
  // point aq = q - fv[0];

  // double U[3][3];
  // make_matrix(ab, ac, -cr, U);
  // print_matrix(U);

  // double U_1[3][3];
  // bool inversed = inverse(ab, ac, ra, U_1);
  // ASSERT(inversed);
  // auto ret_params = matrix_prd(U_1, aq);

  // alpha = ret_params[0];
  // beta = ret_params[1];

  // return (alpha * ab + beta * ac + fv[0]);




  long long ab[3], ac[3], aq[3], ar[3], ra[3];

  vec_minus_int(a_tri->m_ver[1], a_tri->m_ver[0], ab);
  vec_minus_int(a_tri->m_ver[2], a_tri->m_ver[0], ac);
  vec_cross_int(ab, ac, ar);
  vec_neg_int(ar, ra);

  double U_1[3][3];
  bool inversed = inverse(ab, ac, ra, U_1);
  ASSERT(inversed);

  vec_minus_int(q, a_tri->m_ver[0], aq);
  double alpha = 0.0, beta = 0.0;
  for (int i = 0; i < 3; i++) {
    alpha += U_1[0][i] * aq[i];
    beta += U_1[1][i] * aq[i];
  }

  for (int i = 0; i < 3; i++) {
    proj[i] = (ELEMTYPE) (alpha * double(ab[i]) + beta * double(ac[i]) + double(a_tri->m_ver[0][i]));
  }

  if (0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0 && alpha + beta <= 1.0) {
    return true;
  } else {
    return false;
  }
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqDistPtLine(const ELEMTYPE q[NUMDIMS], const ELEMTYPE s[NUMDIMS], const ELEMTYPE t[NUMDIMS], double& lambda) const
{
  long long st[3], sq[3];
  vec_minus_int(t, s, st);
  vec_minus_int(q, s, sq);

  lambda = double(vec_dot_int(st, sq)) / vec_sq_norm_int(st);
  if (lambda < 0) {
    lambda = 0;
  } else if (lambda > 1) {
    lambda = 1;
  }

  long long qp[3];
  for (int i = 0; i < 3; i++) {
    qp[i] = (long long) (lambda * double(st[i]) - double(sq[i]));
  }
  return (ELEMTYPEREAL) vec_sq_norm_int(qp);

    // point st = t - s;
    // point sq = q - s;
    // lambda = (st DOT sq) / sq_norm(st);
    // if (0 <= lambda && lambda <= 1) {
    //     return sq_norm(lambda * st - sq);
    // } else if (lambda < 0) { // nn point is s
    //     lambda = 0;
    //     return sq_norm(sq);
    // } else if (lambda > 1) { // nn point is t
    //     lambda = 1;
    //     return sq_norm(q - t);
    // }
}

RTREE_TEMPLATE
ELEMTYPEREAL RTREE_QUAL::SqDistPtTri(const ELEMTYPE center[NUMDIMS], const Tri* a_tri) const
{
  // get projection of q to plane defined by fv[3]
  // if projection inside triangle, then the projection distance must be the shortest
  ELEMTYPE proj[NUMDIMS];
  if (CheckPtProjInsideTri(center, a_tri, proj)) { // proj inside triangle
      // res = proj;
      // flag = -1;
      return SqDist(center, proj);
  }

  // then, the shortest distance must appear on edge
  ELEMTYPEREAL min_sq_dist = std::numeric_limits<ELEMTYPE>::max();
  const int comb[3][2] = { { 0, 1 }, { 0, 2 }, { 1, 2 } };
  for (int i = 0; i < 3; i++) {
      double tmp_l;

      const ELEMTYPEREAL tmp_sq_dist = SqDistPtLine(center, a_tri->m_ver[comb[i][0]], a_tri->m_ver[comb[i][1]], tmp_l);
      if (min_sq_dist > tmp_sq_dist) {
          min_sq_dist = tmp_sq_dist;
          // if (tmp_l == 0) {
          //     res = fv[comb[i][0]];
          //     flag = comb[i][0];
          // } else if (tmp_l == 1) {
          //     res = fv[comb[i][1]];
          //     flag = comb[i][1];
          // } else {
          //     res = fv[comb[i][0]] + tmp_l * (fv[comb[i][1]] - fv[comb[i][0]]);
          //     flag = -1;
          // }
      }
  }

  return min_sq_dist;
}

#undef RTREE_TEMPLATE
#undef RTREE_QUAL

#endif //RTREE_H

