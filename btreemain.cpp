#include <iostream>
#include <fstream>
#include <cstring>

template <typename key_t>
class RowData{
    key_t x{};

    friend class BPTNode;
    friend class BPTree;

public:
    RowData() = default;

    void setData(key_t x_){
        this->x = x_;
    }

    void displayData() {
        printf("Data: %d\n", x);
    }

    bool operator < (RowData const &obj) {
        return x < obj.x;
    }

    bool operator <= (RowData const &obj) {
        return x <= obj.x;
    }

    bool operator == (RowData const &obj) {
        return x == obj.x;
    }
};

template <typename key_t>
class BPTNode {
	bool isLeaf;
    int size;
	RowData<key_t>* data;
	BPTNode** ptr;

    friend class BPTree;

public:
	explicit BPTNode(int t) {
		isLeaf = false;
		ptr = new BPTNode*[2*t];
		data = new RowData[2*t-1];
		size=1;

		for(int i = 0; i <= (2*t); ++i) {
			ptr[i] = nullptr;
		}
	}

	~BPTNode() {
	    printf("BPTNode Destructor\n");
		delete[] data;
		delete[] ptr;
	}
};

struct SearchResult {
	int index; // between t-1 and 2t-1
	BPTNode* bptNode;

	SearchResult(){
		index = -1;
		bptNode = nullptr;
	}
};

class BPTree {
    BPTNode* root;
    int t;

    int binSearch(BPTNode* temp, RowData* keyData) {
		int l = 0;
		int r = temp->size-1;
		int mid;
		int ans = temp->size;

		while(l <= r) {
            mid = (l+r)/2;
			if((*keyData) <= temp->data[mid]) {
				r = mid-1;
				ans = mid;
			}
			else {
				l = mid+1;
			}
		}
		return ans;
	}

public:

	BPTree(int t_):t(t_), root(nullptr){}

	SearchResult search(RowData* keyData){
		SearchResult searchRes;

		if(root != nullptr){
			BPTNode* temp = root;

			while(!(temp->isLeaf)) {
				int indexFound = binSearch(temp, keyData);
				temp = temp->ptr[indexFound];
			}

			int indexFound = binSearch(temp, keyData);
			if((indexFound != temp->size) && (temp->data[indexFound] == *keyData)){
				// Found
                searchRes.index = indexFound;
                searchRes.bptNode = temp;
			}
		}
        return searchRes;
	}

	bool insert(RowData* keyData) {
		if(root == nullptr) {
			root = new BPTNode(t);
			root->data[0] = *keyData;
			root->isLeaf = true;
			return true;
		}

        // If root is full create a new root and split this root
        if(root->size == (2*t-1)) {
            BPTNode* newRoot = new BPTNode(t);
            BPTNode* nBnode = new BPTNode(t);
            if (root->isLeaf) {
                newRoot->isLeaf = true;
                nBnode->isLeaf = true;
            }
            for(int i = t; i <= 2*t-2; ++i) {
                nBnode->data[i-t] = root->data[i];
                nBnode->ptr[i-t] = root->ptr[i];
            }

            newRoot->size = 1;
            newRoot->data[0] = root->data[t-1];
            newRoot->ptr[1]=nBnode;
            newRoot->ptr[0]=root;
            nBnode->ptr[t-1]=root->ptr[2*t-1];
            nBnode->size = t-1;

            if(!(root->isLeaf)) {
                root->size = t-1;
                root->ptr[t]=nullptr;
            }
            else {
                root->size = t;
            }
            root = newRoot;
            root->isLeaf = false;
        }

        BPTNode* temp = root;
        BPTNode *child;

        while(!temp->isLeaf) {
            int indexFound = binSearch(temp, keyData);
            child = temp->ptr[indexFound];

            if(child->size < (2*t-1)) {
                temp = child;
                continue;
            }

            // Child is full, split it first and then go down
            for(int i=temp->size-1;i>=indexFound;i--) {
                temp->data[i+1] = temp->data[i];
                temp->ptr[i+2] = temp->ptr[i+1];
            }

            temp->data[indexFound] = child->data[t-1];
            BPTNode* nBnode = new BPTNode(t);
            nBnode->isLeaf = child->isLeaf;

            for(int i=t;i<=(2*t-2);i++) {
                nBnode->data[i-t] = child->data[i];
                nBnode->ptr[i-t] = child->ptr[i];
            }

            temp->ptr[indexFound+1]=nBnode;
            nBnode->ptr[t-1]=child->ptr[2*t-1];
            nBnode->size = t-1;
            temp->size++;

            if(!(child->isLeaf)) {
                child->size = t-1;
            }
            else {
                child->size = t;
            }

            child->ptr[t]=nullptr;

            if((*keyData) <= temp->data[indexFound]) {
                temp = temp->ptr[indexFound];
            }
            else {
                temp = temp->ptr[indexFound+1];
            }
        }

        int insertAtIndex = 0;
        for(int i = temp->size-1; i >= 0; --i){
            if((*keyData) <= temp->data[i]) {
                temp->data[i+1]=temp->data[i];
            }
            else {
                insertAtIndex=i+1;
                break;
            }
        }

        temp->data[insertAtIndex] = *keyData;
        temp->size++;
        return true;
	}

	bool deleteAtLeaf(BPTNode* temp, int index) {
		if(root->isLeaf && root->size == 1) {
			root->~BPTNode();
			root = nullptr;
			return true;
		}
		else {
//			 for(int i=0;i<temp->size;i++) {
//			 	cout<<temp->data[i].x<<" ";
//			 }cl;cl;
			for(int i=index;i<temp->size-1;i++) {
				temp->data[i] = temp->data[i+1];
			}
			temp->size--;
			return true;
		}

		// else {
		// 	BPTNode* lchild = temp->ptr[index];
		// 	BPTNode* rchild = temp->ptr[index+1];

		// 	if(lchild->size > t-1) {
		// 		temp->data[index] = lchild->data[lchild->size-1];
		// 		return findAndDelete(&(temp->data[index]), lchild);
		// 	}
		// 	else if(rchild->size > t-1) {
		// 		temp->data[index] = rchild->data[0];
		// 		return findAndDelete(&(temp->data[index]), rchild);
		// 	}
		// 	else {

		// 	}
		//  }
	}

	bool findAndDelete(RowData* keyData) {
		if(root == nullptr) {
			return false;
		}

		BPTNode* temp = root;
		BPTNode *child;


		while( !(temp->isLeaf) ) {

			int indexFound = binSearch(temp, keyData);

			child = temp->ptr[indexFound];

			if (child->size == (t-1)) {
				// If child is of size t-1 fix it and then traverse in
				bool flag=0;
				if(indexFound>0) {
					// check left sibling of child
					BPTNode* lsibling = temp->ptr[indexFound-1];
					if(lsibling->size > t-1) {
						if(child->isLeaf){
							for(int i=child->size-1;i>=0;i--){
								child->data[i+1]=child->data[i];
							}
							temp->data[indexFound-1] = lsibling->data[lsibling->size-1];
							child->data[0]=temp->data[indexFound-1];

						}
						else {
							for(int i=child->size-1;i>=0;i--){
								child->data[i+1]=child->data[i];
								child->ptr[i+2]=child->ptr[i+1];
							}
							child->ptr[1]=child->ptr[0];
							child->data[0]=temp->data[indexFound-1];
							child->ptr[0]=lsibling->ptr[lsibling->size];
							lsibling->ptr[lsibling->size]=nullptr;
							temp->data[indexFound-1] = lsibling->data[lsibling->size-1];
						}
						lsibling->size--;
						child->size++;
						flag=1;
						temp = child;

					}

				}

				if(flag == 0 && indexFound < temp->size){

					BPTNode* rsibling = temp->ptr[indexFound+1];
					if(rsibling->size > t-1) {
						if (child->isLeaf) {
							temp->data[indexFound] = rsibling->data[0];
							child->data[child->size]=temp->data[indexFound];
							for(int i=0;i<rsibling->size-1;i++){
								rsibling->data[i]=rsibling->data[i+1];
							}

						}
						else {
							child->data[child->size]=temp->data[indexFound];
							child->ptr[child->size+1]=rsibling->ptr[0];
							temp->data[indexFound] = rsibling->data[0];
							for(int i=0;i<rsibling->size-1;i++){
								rsibling->data[i]=rsibling->data[i+1];
								rsibling->ptr[i]=rsibling->ptr[i+1];
							}
							rsibling->ptr[rsibling->size-1]=rsibling->ptr[rsibling->size];

						}

						child->size++;
						rsibling->size--;
						flag=1;
						temp = child;

					}
				}

				if(flag==0) {
					if(indexFound>0) {
						BPTNode* lsibling = temp->ptr[indexFound-1];
						//lsibling->data[t] = temp->data[indexFound-1];
						lsibling->ptr[t-1] = child->ptr[0];

						for(int i=0;i<child->size;i++) {
							lsibling->data[t+i-1] = child->data[i];
							lsibling->ptr[t+i] = child->ptr[i+1];
						}

						for(int i=indexFound-1;i<temp->size-1;i++){
							temp->data[i]=temp->data[i+1];
							temp->ptr[i+1]=temp->ptr[i+2];
						}
						temp->ptr[temp->size]=nullptr;
						temp->size--;
						if(temp->size==0) {
							// happens only when temp is root
							temp->~BPTNode();
							root = lsibling;
						}
						flag=1;
						temp = lsibling;
						lsibling->size = (2*t-2);
						child->~BPTNode();
					}
					else if(indexFound < temp->size){
						BPTNode* rsibling = temp->ptr[indexFound+1];
						child->ptr[t-1] = rsibling->ptr[0];

						for(int i=0;i<rsibling->size;i++) {
							child->data[t+i-1] = rsibling->data[i];
							child->ptr[t+i] = rsibling->ptr[i+1];
						}
						child->size=(2*t-2);


						for(int i=indexFound;i<temp->size-1;i++){
							temp->data[i]=temp->data[i+1];
							temp->ptr[i+1]=temp->ptr[i+2];
						}
						temp->ptr[temp->size]=nullptr;
						temp->size--;
						if(temp->size==0) {
							// happens only when temp is root
							temp->~BPTNode();
							root = child;
						}
						flag=1;
						temp = child;
						rsibling->~BPTNode();
					}
					else {
						cout<<"FUCK"<<endl;
					}
				}

				if(flag==0) cout<<"FUCKKKKKKKKKKK"<<endl;

			}
			else {
				temp = child;
			}
		}

		// Now we are in a leaf node
		int indexFound = binSearch(temp, keyData);
//		o3("F", keyData->x, indexFound);
		if(indexFound < temp->size) {
			if (temp->data[indexFound] == *(keyData)) {
				return deleteAtLeaf(temp,indexFound);
			}
		}

		return false;
	}

    void bfsTraverse() {
        bfsTraverseUtil(root);
    }

	void bfsTraverseUtil(BPTNode* start) {
		if(start != nullptr) {
             cout<<start->size<<"# ";
			 for(int i=0;i<start->size;i++) {
			 	cout<<start->data[i].x<<" ";
			 }cout<<endl;

			if (!(start->isLeaf)) {
				for(int i=0;i<start->size+1;i++) {
					bfsTraverseUtil(start->ptr[i]);
				}
			}

		}
	}
};


//int fun(int x){
//    cout << "FUN INT" << endl;
//}

//void fun(const char* x){
//    cout << "FUN1" << endl;
//}

void fun(std::string x){
    cout << "FUN2" << endl;
}


int main(){
//    fun("HELLO");
//    a.print();

	 BPTree bt(3);
	 RowData rd;
	 rd.setData(10);
	 bt.insert(&rd);
    bt.bfsTraverse();
	 rd.setData(20);
	 bt.insert(&rd);
    bt.bfsTraverse();
	 rd.setData(5);
	 bt.insert(&rd);
    bt.bfsTraverse();
	 rd.setData(15);
	 bt.insert(&rd);
    bt.bfsTraverse();
	 bt.bfsTraverse();
	 rd.setData(11);
	 bt.insert(&rd);
	 bt.bfsTraverse();
	 rd.setData(21);
	 bt.insert(&rd);
	 bt.bfsTraverse();
	 rd.setData(51);
	 bt.insert(&rd);
	 bt.bfsTraverse();
	 rd.setData(17);
	 bt.insert(&rd);
	 bt.bfsTraverse();
	 rd.setData(71);
	 bt.insert(&rd);
	 bt.bfsTraverse();

     cout<<"Insert done"<<endl;
      rd.setData(51);
	  SearchResult rs = bt.search(&rd);
	  cout<<rs.index<<endl;

	  cout<<bt.findAndDelete(&rd)<<endl;
	  bt.bfsTraverse();
      rd.setData(71);
	  cout<<bt.findAndDelete(&rd)<<endl;
	  bt.bfsTraverse();
	  rd.setData(51);
	  cout<<bt.findAndDelete(&rd)<<endl;
	  bt.bfsTraverse();
	  rd.setData(21);

	  cout<<bt.findAndDelete(&rd)<<endl;
	  bt.bfsTraverse();
}

