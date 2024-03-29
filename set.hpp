/*
    Starter code for assignment 6, CSC 2/454, Fall 2019
    Provides skeleton of code for a simple hierarchy of set
    abstractions.
    Do not include a `main` function with your submission.
    (Note: Extra Credit - Add documentation that looks great with `doxygen`)
*/

#include <set>
#include <iostream>
#include <string.h>
#include <type_traits>
#include <sstream> 
#include <bitset>
using std::set;
using std::cout;
using std::string;

/// Naive comparator.
/// Provides a default for any type that has an operator<
/// and an operator==.
///
template<typename T>
class comp {
  public:
    bool precedes(const T& a, const T& b) const {
        return a < b;
    }
    bool equals(const T& a, const T& b) const {
        return a == b;
    }
};

/*!
* Abstract base class from which all sets are derived.
*/
template<typename T, typename C = comp<T>>
class simple_set {
  public:
    /**
     * \brief Destructor
     * 
     * Destructor should be virtual so that we call the right version when saying, e.g.,
     * 
     * \code{.cc}
     * simple_set* S = new derived_set(args);
     * // ...
     * delete S;
     * \endcode
     * 
     */
    virtual ~simple_set<T, C>() { }
    
    /** 
     * \brief Inserts item into set
     * 
     * Returns a ref so you can say, e.g.
     * 
     * \code{.cc}
     * S += a += b += c;
     * \endcode
     * 
     */
    virtual simple_set<T, C>& operator+=(const T item) = 0;
        
    /**
     * \brief Removes item from set, if it was there (otherwise does nothing)
     * 
     * Returns a ref so you can say, e.g.
     * 
     * \code{.cc}
     * S -= a -= b -= c;
     * \endcode
     * 
     */
    virtual simple_set<T, C>& operator-=(const T item) = 0;
    
    /**
     * \brief Indicates whether item is in set
     * 
     * Indicates whether item is in set.
     */
    virtual bool contains(const T& item) const = 0;
};

//---------------------------------------------------------------

/** 
 * Example of a set that implements the simple_set interface.
 * Borrows the balanced tree implementation of the standard template
 * library.  Note that you are NOT to use any standard library
 * collections in your code (though you may use strings and streams).
 * 
 * 'virtual' on simple_set ensures single copy if multiply inherited
 */
template<typename T>
class std_simple_set : public virtual simple_set<T>, protected set<T> {
  public:
    /// will invoke std::~set<T>()
    virtual ~std_simple_set<T>() { }  
    virtual std_simple_set<T>& operator+=(const T item) {
        set<T>::insert(item);
        return *this;
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        (void) set<T>::erase(item);
        return *this;
    }
    virtual bool contains(const T& item) const {
        return (set<T>::find(item) != set<T>::end());
    }
};

//---------------------------------------------------------------

//USED ONLY FOR TESTING
template<typename T>
static std::string toBinaryString(const T& x)
{
    std::stringstream ss;
    ss << std::bitset<sizeof(T) * 8>(x);
    return ss.str();
}

/// Exception
class out_of_bounds { };    

/** 
 * Characteristic array implementation of set.
 * Requires instantiation with guaranteed low and one-more-than-high
 * bounds on elements that can be placed in the set.  Should compile
 * and run correctly for any element class T that can be cast to int.
 * Throws out_of_bounds exception when appropriate.
 */
template<typename T>
class carray_simple_set : public virtual simple_set<T> {
    /// 'virtual' on simple_set ensures single copy if multiply inherited
    /// You'll need some data members here.
    T low;
    T high;
    int numWords;
    long * arr;

  public:
    /// fill in these methods:
    
    /// constructor
    carray_simple_set(const T l, const T h) {   
         low = l;
        high = h;
        numWords = ((high - low)/64) + 1;
        arr = new long[numWords];
        for(int i = 0; i< numWords; i++){
            arr[i] = 0;
        }
    }

    /// destructor
    virtual ~carray_simple_set() {              
        delete arr;
    }
    virtual carray_simple_set<T>& operator+=(const T item) {
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        int word_index = item/64;
        int bit_index = item%64;
        arr[word_index] |= 1UL << bit_index;
        return *this;
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        if(item < low || item >= high){
            throw out_of_bounds();
        }
        int word_index = item/64;
        int bit_index = item%64;
        arr[word_index] &= ~(1UL << bit_index);
        return *this;
    }
    virtual bool contains(const T& item) const {
         int word_index = item/64;
        int bit_index = item%64;
        bool contained = ((arr[word_index] >> bit_index) & 1U) == 1; 
        return contained;
    }

    void print(){
        for(int i = 0; i<numWords; i++){
            cout << toBinaryString(arr[i]) << std::endl;
        }
    }
};

//---------------------------------------------------------------

///
/// Naive hash function object.
/// Provides a default for any type that can be cast to int.
///
template<typename T>
class cast_to_int {
  public:
    int operator()(const T n) {
        return (int) n;
    }
};

/// Exception
class overflow { };         

///
/// Hash table implementation of set.
/// Requires instantiation with guaranteed upper bound on number of elements
/// that may be placed in set.  Throws overflow if bound is exceeded.
/// Can be instantiated without second generic parameter if element type
/// can be cast to int; otherwise requires hash function object.
///
template<typename T, typename F = cast_to_int<T>>
class hashed_simple_set : public virtual simple_set<T> {
    /// 'virtual' on simple_set ensures single copy if multiply inherited
    ///
    /// You'll need some data members here.
    ///
    /// I recommend you pick a hash table size p that is a prime
    /// number >= n, use F(e) % p as your hash function, and rehash
    /// with kF(e) % p after the kth collision.  (But make sure that
    /// F(e) is never 0.)
    struct hashnode {
    T value;
    hashnode* next;
    };

    struct hashnode* createHashNode(T value)
    {
    struct hashnode* newNode = (struct hashnode*)malloc(sizeof(struct hashnode));
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
    };

    hashnode** hashtable;
    int max_count;
    int current_count;
    int table_size;	

    public:
    //Constructor
    hashed_simple_set(const int n) {   
    hashtable = new hashnode*[n];
    table_size = n;
    for (int i = 0; i < table_size; i++)
    {
    hashtable[i] = NULL;
    }

    (void) n;
    }

    //Destructor
    virtual ~hashed_simple_set() 
    {
        for (int i = 0; i < table_size; ++i){
            hashnode* each = hashtable[i];
            while(each) {
                hashnode* prev = each;
                each = each->next;
                delete prev;
            }
        }
    delete[] hashtable;
    } 

    /* Data structure methods */
    int hash(int value) const
    {
        return value % table_size;
    }

    virtual hashed_simple_set<T, F>& operator+=(const T item) {
        int hashed = hash(item);//Passive call to operator?
        hashnode* last_node = NULL;
        hashnode* current_node = hashtable[hashed];//Find node at correct bucket
        while(current_node)
        {
            last_node = current_node;
            current_node = current_node->next;
        }

        if(!current_node){
            current_node = createHashNode(item);
            if (!last_node)
            {
                hashtable[hashed] = current_node;
            }
            else 
            {
                last_node->next = current_node;
            }
        } 
        else 
        {
            current_node->value = item;
        }

        (void) item;  return *this;
    }

virtual hashed_simple_set<T, F>& operator-=(const T item) {
    int hashed = hash(item);//Passive call to operator?
	hashnode* last_node = NULL;
	hashnode* current_node = hashtable[hashed];//Find node at correct bucket
	
	while(current_node)
	{
	  if (current_node->value == item)
	  {
	    if(last_node)
	    {
	      last_node->next = current_node->next;
	    }
	   delete current_node;
           (void) item; return *this;	  }
	   current_node = current_node->next;
	}  
	(void) item;  return *this;
    }

    virtual bool contains(const T& item) const { 
	int hashed = hash(item);
	hashnode* current_node = hashtable[hashed];
	while (current_node)
	{
	  if(current_node->value == item) { (void) item; return true; }
	  current_node = current_node->next;
	}	
        (void) item;  return false;
    }
};

//---------------------------------------------------------------

/// Sorted array implementation of set; supports binary search.
/// Requires instantiation with guaranteed upper bound on number of
/// elements that may be placed in set.  Throws overflow if bound is
/// exceeded.
///
/// 'virtual' on simple_set ensures single copy if multiply inherited
template<typename T, typename C = comp<T>>
class bin_search_simple_set : public virtual simple_set<T> {
    struct node {
		T value;
		node* left;
		node* right;
	};
	
	C cmp;
	int current_count = 0;
	int max_count;

  public:
    struct node* root; 

    bin_search_simple_set(const double n){
            max_count = n;
	    (void) n;
    }
    virtual ~bin_search_simple_set<T, C>() { }

    /* Data structure methods */
    
    //Node creation
    struct node* createNode(T value)
    {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        newNode->value = value;
        newNode->left = NULL;
       	newNode->right = NULL;
        return newNode;
    }

    //In-order print of the binary search tree
    void printBST(struct node* root)
    {
        if (root)
        {
            printBST(root->left);
            printf("%d ", root->value);
            printBST(root->right);
        }
    }
    
    //Attempts to insert a new node at a given subtree
    struct node* r_insert(struct node* node, T value)
    {
        if (!node)
        {
	    //Check bounds for insertion
	    if(current_count == max_count) throw overflow();
            current_count++;
	    struct node* newnode = createNode(value);
	    return newnode; //Empty case
        }

        if (cmp.precedes(value, node->value))
            node->left = r_insert(node->left, value);
        else
            node->right = r_insert(node->right, value);

        return node;
    }

    //Finds a subtree's smallest value node, used for deletion mostly
    struct node* smallestNode(struct node* node)
    {
        struct node* current = node;

        while (current && current->left)//Iterates until it's at the bottom left
            current = current->left;

        return current;
    }

    //Recursivley attempts to remove a node of given value within the subtree
    struct node* r_deleteNode(struct node* root, T value)
    {
        // No item found, do nothing
        if (!root) return root;

        //At target value
        if (cmp.equals(value, root->value)) 
        { 
	    current_count--;//Removing item reduces count
            if (!root->left)//Only right child present
            {
                struct node* temp = root->right;
                free(root);
                return temp;
            }
            else if (!root->right)//Only left child present
            {
                struct node* temp = root->left;
                free(root);
                return temp;
            }

            //Both nodes present
            struct node* temp = smallestNode(root->right);//Find replacement value
            root->value = temp->value;//Replace value
            root->right = r_deleteNode(root->right, temp->value);//Remove the old ref to replacer
        }
        //Smaller value
        else if (cmp.precedes(value, root->value)){ 
		root->left = r_deleteNode(root->left, value);
	}
        //Must be larger value
        else
        {
           root->right = r_deleteNode(root->right, value);
        }
        return root;
    }

    bool r_contains(struct node* root, T value) const
    {
	
	if (!root) return false;
	if (cmp.equals(root->value, value)) return true;
	
	if (cmp.precedes(value, root->value))
	   return r_contains(root->left, value);
    	else return r_contains(root->right, value);
    }

	// inserts item into set
	// returns a ref so you can say, e.g.
	// S += a += b += c;
    virtual bin_search_simple_set<T, C>& operator+=(const T item){
        if (root)
        {
            r_insert(root, item);
        }
        else //If there's no root yet, assign it
        {
	    root = r_insert(root, item);
        }

        (void) item; return *this;
    }

	// removes item from set, if it was there (otherwise does nothing)
    virtual bin_search_simple_set<T, C>& operator-=(const T item) {
        if (root)
        {
            r_deleteNode(root, item);
        }

        (void) item; return *this;
    }

	// and some methods
    virtual bool contains(const T& item) const {
        (void) item; return r_contains(root, item);  
    }
};

//===============================================================
// RANGE SETS

/// Function object for incrementing.
/// Provides a default for any integral type.
///
template<typename T>
class increment {
    static_assert(std::is_integral<T>::value, "Integral type required.");
  public:
    T operator()(T a) const {
        return ++a;
    }
};

/// Range type.  Uses comp<T> by default, but you can provide your
/// own replacement if you want, e.g. for C strings.
///
static const struct { } empty_range;    // exception

/// No destructor needed. You may also find it useful to define the following:
/// 
/// \code{.cc}
/// bool precedes(const range<T, C>& other) const { ...
/// bool overlaps(const range<T, C>& other) const { ...
/// \endcode
template<typename T, typename C = comp<T>>
class range {
    /// represents all elements from L
    T L;        
    /// inclusive?
    bool Linc;  
    /// through H
    T H;        
    /// inclusive?
    bool Hinc;  
    /// can't be static; needs explicit instantiation
    C cmp;      
  public:
    range(const T l, const bool linc, const T h, const bool hinc)
            : L(l), Linc(linc), H(h), Hinc(hinc), cmp() {
        if (cmp.precedes(h, l)
            || (cmp.equals(l, h) && (!Linc || !Hinc))) throw empty_range;
    }
    
    T low() const { return L; }
    bool closed_low() const { return Linc; }
    T high() const { return H; }
    bool closed_high() const {return Hinc; }
    bool contains(const T& item) const {
        return ((cmp.precedes(L, item) || (Linc && cmp.equals(L, item)))
            && (cmp.precedes(item, H) || (Hinc && cmp.equals(item, H))));
    }
    
};

// You may find it useful to define derived types with two-argument
// constructors that embody the four possible combinations of open and
// close-ended:
//
// template<typename T, typename C = comp<T>>
// class CCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class COrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OCrange : public range<T, C> { ...
// 
// template<typename T, typename C = comp<T>>
// class OOrange : public range<T, C> { ...

/// This is the abstract class from which all range-supporting sets are derived.
///
template<typename T, typename C = comp<T>>
class range_set : public virtual simple_set<T> {
    // 'virtual' on simple_set ensures single copy if multiply inherited
  public:
    virtual range_set<T, C>& operator+=(const range<T, C> r) = 0;
    virtual range_set<T, C>& operator-=(const range<T, C> r) = 0;
};

//---------------------------------------------------------------

/// As implemented in the standard library, sets contain individual
/// elements, not ranges.  (There are range insert and erase operators, but
/// (a) they use iterators, (b) they take time proportional to the number of
/// elements in the range, and (c) they require, for deletion, that the
/// endpoints of the range actually be in the set.  An std_range_set, as
/// defined here, avoids shortcomings (a) and (c), but not (b).  Your
/// bin_search_range_set should avoid (b), though it will have slow insert
/// and remove operations.  A tree_range_set (search tree -- extra credit)
/// would have amortized log-time insert and remove for individual elements
/// _and_ ranges.
///
template<typename T, typename C = comp<T>, typename I = increment<T>>
class std_range_set : public virtual range_set<T, C>,
                      public std_simple_set<T> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    static_assert(std::is_integral<T>::value, "Integral type required.");
    I inc;
  public:
    // The first three methods below tell the compiler to use the
    // versions of the simple_set methods already found in std_simple_set
    // (given true multiple inheritance it can't be sure it should do that
    // unless we tell it).
    virtual std_simple_set<T>& operator+=(const T item) {
        return std_simple_set<T>::operator+=(item);
    }
    virtual std_simple_set<T>& operator-=(const T item) {
        return std_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return std_simple_set<T>::contains(item);
    }
    virtual range_set<T>& operator+=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this += i;
        }
        return *this;
    }
    virtual range_set<T>& operator-=(const range<T, C> r) {
        for (T i = (r.closed_low() ? r.low() : inc(r.low()));
                r.contains(i); i = inc(i)) {
            *this -= i;
        }
        return *this;
    }
};

//---------------------------------------------------------------

template<typename T, typename C = comp<T>, typename I = increment<T>>
class carray_range_set : public virtual range_set<T, C>, public carray_simple_set<T> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    static_assert(std::is_integral<T>::value, "Integral type required.");
    I inc;
  public:
    
    //Barebones constructor
    carray_range_set(const T l, const T h) : carray_simple_set<T>(l,h) {
    }
    
    //Call inherited methods
    virtual carray_simple_set<T>& operator+=(const T item) {
    	return carray_simple_set<T>::operator+=(item);
    }
    virtual carray_simple_set<T>& operator-=(const T item) {
        return carray_simple_set<T>::operator-=(item);
    }
    virtual bool contains(const T& item) const {
        return carray_simple_set<T>::contains(item);
    }
    
    //Use incrementing insertion
    virtual carray_range_set<T, C, I>& operator+=(const range<T, C> r){
    
    	for (T i = (r.closed_low() ? r.low() : inc(r.low()));
    	           r.contains(i); i = inc(i)) {
    		*this += i;
    		//carray_simple_set<T>& operator+=(const T item);
    	}
    	return *this;
    }
    
    //Increment for deletion
    virtual carray_range_set<T, C, I>& operator-=(const range<T, C> r){
    	for (T i = (r.closed_low() ? r.low() : inc(r.low()));
    	         r.contains(i); i = inc(i)) {
    		if(this->contains(i))
    			*this -= i;
    	}
    	return *this;
    }
};

//---------------------------------------------------------------

/// Fill out
template<typename T, typename F = cast_to_int<T>, typename C = comp<T>, typename I = increment<T>>
class hashed_range_set : public virtual range_set<T, C> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    static_assert(std::is_integral<T>::value, "Integral type required.");
    I inc;
  public:
    /**
     * @throws overflow
     */
    virtual hashed_range_set<T, F, C, I>& operator+=(const range<T, C> r) = 0;
};
//---------------------------------------------------------------

/// Fill out
template<typename T, typename C = comp<T>>
class bin_range_set : public virtual range_set<T, C> {
    // 'virtual' on range_set ensures single copy if multiply inherited
    range<T> arr[10] = {};
    int max_count = 10;
    int current_count = 0;
    C cmp;

    public:
    bin_range_set<T,C>(const double n){
        //max_count = n;
	(void) n;
    }

    void deleteByIndex(int index); //Decrement current_count if successful
    void insertAtIndex(range<T> newrange, int index) //Increment current_count if successful
    {
      // Cannot insert more elements if n is already 
      // more than or equal to capcity 
      if (current_count+1 == max_count) throw overflow();
  
      int i; 
      for (i = current_count - 1; (i >= 0 && i > index); i--) 
          arr[i + 1] = arr[i]; 
  
      arr[i + 1] = newrange; 
  
      return; 	
    }
    //Returns 0 if in range, 1 if greater than range, -1 if less than range
    int cmpToRange(range<T> r, T value)
    {
	//Is T within the left bound of the range?
	bool inleft = (r.closed_low ? (cmp.precedes(r.low, value) || cmp.equals(r.low, value)) : cmp.precedes(r.low, value));
	//Right bound?	
	bool inright = (r.closed_high ? (cmp.precedes(value, r.high) || cmp.equals(value, r.high)) : cmp.precedes(value, r.low));

	if (inleft && inright) return 0;
	else if (inleft && !inright) return 1;
	else if (!inleft && inright) return -1;
	else { printf("This shouldn't happen!"); return 100; }
    }

    //Finds index of element if it exists in the ranges, otherwise returns -1;
    int findIndex(int l, int r, T target)
    {
      
      if (r >= l) 
      { 
        int mid = l + (r - l) / 2; 
  
        //Found it at this midpoint
        if (cmpToRange(arr[mid], target) == 0)//In range 
            return mid; 
  
        //Element is smaller than midpoint and must be in first half
        if (cmpToRange(arr[mid], target) == -1) 
            return findIndex(arr, l, mid - 1, target); 
  
        //Element is bigger than midpoint and must be in second half
        return findIndex(arr, mid + 1, r, target); 
      } 
  
    // Element not in array 
    return -1; 
	
    }

    int findClosestLeftIndex(int l, int r, T target);
    int findClosestRightIndex(int l, int r, T target);
    	

    /**
     * @throws overflow
     */
    virtual bin_range_set<T, C>& operator+=(const range<T, C> r)
    {
	int indexOfLeft = findIndex(0, current_count-1, r.low);
	int indexOfRight = findIndex(0, current_count-1, r.high);
	
	if (indexOfLeft == -1 && indexOfRight == -1)//Neither end in set
	{
		int CL = GetClosestLeftIndex(0, current_count-1, r.low);//Find the range closest left of r.low
		int CR = GetClosestRightIndex(0, current_count-1, r.high);//Same for right
		if ((CR-CL) <= 1) insertAtIndex(r, CL+1); //In an empty gap between two ranges
		else
		{
		  for(int i = 0; i < CR-CL-1; i++)//Eat all ranges between the original CL and CR ranges
		  {
		    deleteByIndex(CL+1);//Later indicies will be shuffled down one by one towards CL+1
		  }

		  insertAtIndex(r, CL+1); //Add new range at CL+1		
		}
	}
	else if (indexOfLeft != -1 && indexOfRight == -1)//Only right side isn't in a range
	{
		  int LI = findIndex(0, current_count-1, r.low);//Index of range that r.low is in
		  int CR = GetClosestRightIndex(0, current_count-1, r.high);//Index of closest set to the right of r.high
		  arr[LI].high = r.high; arr[LI].closed_high = r.closed_high;
		  if ((CR-LI) > 1)
		  {
		    for(int i = 0; i < CR-LI-1; i++)//Eat all ranges between the original LI and CR ranges
		    {
			deleteByIndex(LI+1);//Later indicies will be shuffled down one by one towards LI+1
		    }
		  }
	}
	else if (indexOfLeft == -1 && indexOfRight != -1)//Only left side isn't in a range
	{
		  int RI = findIndex(0, current_count-1, r.high);//Index of range that r.high is in
		  int CL = GetClosestLeftIndex(0, current_count-1, r.low);//Index of closest set to the left of r.low
		  arr[RI].low = r.low; arr[RI].closed_low = r.closed_low; 
		  if ((RI-CL) > 1)
		  {
		    for(int i = 0; i < RI-CL-1; i++)//Eat all ranges between the original RI and CL ranges
		    {
			deleteByIndex(CL+1);//Later indicies will be shuffled down one by one towards CL+1
		    }
		  }
	}
	else
	{
	  	arr[indexOfLeft].high = arr[indexOfRight].high;
		arr[indexOfLeft].closed_high = arr[indexOfRight].closed_high;
		for(int i = 0; i < indexOfRight-indexOfLeft; i++)//Eat all ranges between the original RI and LI ranges
		{
		  deleteByIndex(indexOfLeft+1);//Later indicies will be shuffled down one by one towards CL+1
		}
	}
    }
    
    virtual bin_range_set<T, C>& operator-=(const range<T, C> r) = 0;
    virtual bool contains(const T& item) const = 0;    
};

//===============================================================

/// comparator for C strings
///
class lexico_less {
  public:
    bool precedes(const char *a, const char *b) const {
        return strcmp(a, b) < 0;
    }

    bool equals(const char *a, const char *b) const {
        return strcmp(a, b) == 0;
    }
};


typedef enum{mon, tue, wed, thu, fri} weekday;
