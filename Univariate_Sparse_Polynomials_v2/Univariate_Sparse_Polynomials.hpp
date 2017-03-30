#include <iostream>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define DEFAULT_SIZE 10

using namespace std;

class UnivariateSparsePolynomials
{
private:
	// Declare the node of the doubly linked list
	struct Term
	{
		static Term* freelist;

		int coef;		// Coefficient of the term
		int expo;		// Exponent of the term

		Term *next, *prev;		// Pointers

		Term(const int item_A, const int item_B, Term* prevp, Term* nextp)
		{
			coef = item_A;
			expo = item_B;
			prev = prevp;
			next = nextp;
		}

		Term(Term* prevp = NULL, Term* nextp = NULL)
		{
			prev = prevp;
			next = nextp;
		}

		// Overloaded new operator
		void* operator new(size_t)
		{
			if (freelist == NULL) return ::new Term;
			Term* temp = freelist;
			freelist = freelist->next;
			return temp;
		}

		// Overloaded delete operator
		void operator delete(void* ptr)
		{
			((Term*)ptr)->next = freelist;
			freelist = (Term*)ptr;
		}

	} *curr, *head, *tail;				// Declare the pointers
	int cnt;							// The length of the list

	Term* Term::freelist = NULL;

	// Intialization helper method
	void init()
	{
		curr = head = new Term;
		head->next = tail = new Term(head, NULL);
		cnt = 0;
	}

	// Return link nodes to free stor
	void removeAll()
	{
		while (head != NULL) {
			curr = head;
			head = head->next;
			delete curr;
		}
	}

	void assert(bool val, string s)
	{
		if (!val) {
			cout << "Assertion Failed: " << s << endl;
			exit(-1);
		}
	}

public:
	// Constructor & Destructor
	UnivariateSparsePolynomials(int size = DEFAULT_SIZE) { init(); };
	~UnivariateSparsePolynomials() { removeAll(); };

	void clearTheList() { removeAll(); init(); }		// Clear list
	int getLength() { return cnt; }						// Get the length of the list

	void moveToStart() { curr = head; }					// Place curr at list start
	void moveToEnd() { curr = tail->prev; }				// Place curr at list end

	// Move down list to "pos" position 
	void moveToPos(const int pos)
	{
		assert((pos >= 0) && (pos <= cnt), "Position out of range");
		curr = head;
		for (int i = 0; i < pos; i++) curr = curr->next;
	}

	// Move fence one step right, no change if right is empty
	void next()
	{
		if (curr != tail->prev)
			curr = curr->next;
	}

	// Move fence one step left, no change if left is empty
	void prev()
	{
		if (curr != head)
			curr = curr->prev;
	}

	// Insert items at current position
	void insert(const int& item_A, const int& item_B)
	{
		curr->next = curr->next->prev =
			new Term(item_A, item_B, curr, curr->next);
		cnt++;
	}

	// Append items to the end of the list
	void append(const int& item_A, const int& item_B)
	{
		tail->prev = tail->prev->next =
			new Term(item_A, item_B, tail->prev, tail);
		cnt++;
	}

	friend ostream& operator <<(ostream& os, UnivariateSparsePolynomials& poly)
	{
		char plusSign[] = "", minusSign[] = " - ";
		for (int i = 0; i < poly.length(); i++) {
			if (poly.curr->expo == 0) {
				if (poly.curr->coef < 0) {
					os << minusSign << poly.curr->coef;
				} else {
					os << plusSign << poly.curr->coef;
				}
			} else if (poly.curr->expo == 1) {
				if (poly.curr->coef < 0) {
					os << minusSign << poly.curr->coef << "x";
				} else {
					os << plusSign << poly.curr->coef << "x";
				}
			} else {
				if (poly.curr->coef < 0) {
					os << minusSign << poly.curr->coef << "x^" << poly.curr->expo;
				} else {
					os << plusSign << poly.curr->coef << "x^" << poly.curr->expo;
				}
			}
			if (i == 0) {
				plusSign[] = " + ";
			}
		}
		return os;
	}
};