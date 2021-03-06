//
// Created by Pranav Gupta on 06/02/20.
//

#ifndef DBMS_BTREE_H
#define DBMS_BTREE_H

#include <iostream>
#include <cstring>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include "Constants.h"
#include "Table.h"
#include "BPTreeNodeManager.h"
#include "DataTypes.h"

/*
 * -------------------- BPTNode --------------------
 * 1. isLeaf            => bool
 * 2. size              => int32_t
 * 3. leftSibling       => row_t
 * 4. rightSibling      => row_t
 */

template <typename key_t>
class BPTNode: public Page{
    using Node = BPTNode<key_t>;
    using keyRNPair = std::pair<key_t, pkey_t>;
    using manager_t = BPTreeNodeManager<BPTNode<key_t>>;

    bool isLeaf;
    int size;
    row_t leftSibling_;
    row_t rightSibling_;

    key_t* keys;
    pkey_t* pkeys;
    row_t* child;

    template <typename o_key_t>
    friend class BPTree;

    template <typename node_t>
    friend class BPTreeNodeManager;

public:
    static int32_t childOffset;
    static int32_t pKeyOffset;

    // Setters and Getters
    Node* getChildNode(manager_t& manager, int32_t index);
    Node* getRightSibling(manager_t& manager);
    Node* getLeftSibling(manager_t& manager);
    inline void readHeader(int32_t maxSize, int32_t keySize);
    void writeHeader();
    void allocate(int32_t maxSize, int32_t keySize);

    BPTNode(){
        isLeaf = false;
        size = 0;
        leftSibling_ = 0;
        rightSibling_ = 0;
        this->hasUncommitedChanges = true;
    }

    explicit BPTNode(int32_t pageNo):BPTNode(){
        this->pageNo = pageNo;
    }

    ~BPTNode(){}
};

template <typename key_t>
struct SearchResult {
    int index; // between branchingFactor-1 and 2*branchingFactor-1
    BPTNode<key_t>* node;

    SearchResult(){
        index = -1;
        node = nullptr;
    }
};

class BPlusTreeBase{
public:
    int32_t keySize;
    virtual ~BPlusTreeBase() = default;
    virtual void traverseAllWithKey(std::string){}
    virtual bool traverse(const std::function<bool(row_t row)>& callback){return false;}
};

template <typename key_t>
class BPTree: public BPlusTreeBase{
    using Node      = BPTNode<key_t>;
    using result_t  = SearchResult<key_t>;
    using keyRNPair = std::pair<key_t, long long int>;
    using manager_t = BPTreeNodeManager<BPTNode<key_t>>;

    manager_t manager;
    int32_t branchingFactor;

public:
    BPTree(const char* filename, int32_t branchingFactor_, int32_t keySize_);
    bool insert(const std::string& keyStr, pkey_t pkey, row_t row);
    bool search(const std::string& str);
    bool traverse(const std::function<bool(row_t row)>& callback) override;
    bool BFStraverse(const std::function<bool(row_t row)>& callback);
    void traverseAllWithKey(const std::string& strKey, const std::function<void(row_t rowOfCurrent)>& funcToPrint);
    void bfsTraverseDebug();

    /// true  -> (key, pkey) found and deleted
    /// false -> (key, pkey) not found
    bool remove(const std::string& key, const pkey_t pkey);

    /// true  -> all found records deleted
    /// false -> some data inconsistency
    template <typename callback_t>
    bool remove(const std::string& key, const callback_t& callback, const pkey_t pkey = -1);

private:

    result_t searchUtil(const key_t& key, const pkey_t& pKey);
    void incrementLinkedList(result_t& currentPosition);
    void decrementLinkedList(result_t& currentPosition);
    int32_t binarySearch(Node* node, const key_t& key, const pkey_t pkey);
    void splitRoot();
    void splitNode(Node* parent, Node* child, int indexFound);
    void bfsTraverseUtilDebug(Node* start);
    bool traverseUtil(Node* start, const std::function<bool(row_t row)>& callback);
    void naturalJoinBothIndex(Node* rootOfOtherBTree, const std::function<void(row_t rowOfCurrent, row_t rowOfOther)>& funcToPrint);
    void naturalJoinOneIndex(Node* rootOfOtherBTree, const std::function<void(row_t rowOfCurrent, row_t rowOfOther)>& funcToPrint);

//    void greaterThanEquals(const key_t& key);
//    void smallerThanEquals(const key_t& key);
//    void greaterThan(const key_t& key);
//    void smallerThan(const key_t& key);
//    void removeWithKey(const key_t& key);

private:
    // MARK:- HELPER FUNCTIONS
    // Delete Helpers
    row_t deleteAtLeaf(Node* node, int index);
    void borrowFromLeftSibling(int indexFound, Node* parent, Node* child, Node* leftSibling);
    void borrowFromRightSibling(int indexFound, Node* parent, Node* child, Node* rightSibling);
    void mergeWithSibling(int indexFound, Node*& parent, Node* child, Node* leftSibling, Node* rightSibling);
    void removeHelper(const key_t& key, const pkey_t pkey);
    std::pair<key_t,pkey_t> getMax(Node* node);
    // Traverse Helpers
    bool iterateRightLeaf(Node* node, int startIndex, const std::function<bool(row_t row)>& callback);

    // Join Helpers
    BPTNode<key_t>* leftMostLeaf(Node* root);
//   void removeMultipleAtLeaf(Node* leaf, int startIndex, int countToDelete);
//   void iterateLeftLeaf(Node* node, int startIndex);
};

#include "../BTreeNew.cpp"
#endif //DBMS_BTREE_H
