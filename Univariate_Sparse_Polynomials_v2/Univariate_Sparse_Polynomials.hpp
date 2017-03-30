#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#define DEFAULT_SIZE 10

class UnivariateSparsePolynomials
{
private:
	// Declare the node of the doubly linked list
	struct Term
	{
		static Term* freelist;

		int coef;			// Coefficient of the term
		int expo;			// Exponent of the term

		Term *next, *prev;	// Pointers

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

		// Reload the operator = to assign polynomials easily
		Term& operator =(Term& term)
		{
			this->coef = term.coef;
			this->expo = term.expo;
			this->prev = term.prev;
			this->next = term.next;
		}

	} *curr, *head, *tail;	// Declare the pointero
	Term* Term::freelist = NULL;

	int cnt;				// The length of the list

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

	// Exchange two nodes in list
	void exchange(int pos_A, int pos_B)
	{
		Term* pTemp = curr,
			*pntBufferA, *pntBufferB, *pntBufferC, *pntBufferD;
		moveToPos(pos_A);
		pntBufferA = curr->next;
		pntBufferC = pntBufferA->prev;
		pntBufferD = pntBufferB->next;
		moveToPos(pos_B);
		pntBufferB = curr->next;
		pntBufferA->prev->next = pntBufferA->next->prev = pntBufferB;
		pntBufferB->prev = pntBufferA->prev;
		pntBufferB->next = pntBufferA->next;
		pntBufferA->prev = pntBufferC;
		pntBufferA->next = pntBufferD;
		pntBufferC->next = pntBufferD->prev = pntBufferA;
		curr = pTemp;
	}


	// Check and feedback
	void assert(bool val, std::string s)
	{
		if (!val) {
			std::cout << "Assertion Failed: " << s << std::endl;
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
	Term& moveToPos(const int pos)
	{
		assert((pos >= 0) && (pos <= cnt), "Position out of range");
		curr = head;
		for (int i = 0; i < pos; i++) curr = curr->next;
		return *curr;
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
	void insert(Term* term)
	{
		curr->next = curr->next->prev = term;
		cnt++;
	}

	// Append items to the end of the list
	void append(const int& item_A, const int& item_B)
	{
		tail->prev = tail->prev->next =
			new Term(item_A, item_B, tail->prev, tail);
		cnt++;
	}

	// Input the polynomials and save them in list
	void input()
	{
		int coef, expo;
		//std::cout << "Please input a term in the first polynomial: " << std::endl;
		for (int i = 1; ; i++) {
			std::cout << i << ":";
			std::cout << "\tCoefficient:";
			std::cin >> coef;
			if (coef == '\n') {
				break;
			}
			std::cout << "\tExponent:";
			std::cin >> expo;
			append(coef, expo);
		}
	}

	// Sort and format the input polynomials
	void sortPolynomial(int low, int high)
	{
		if (low >= high) return;

		int	first = low;
		int last = high;
		Term key = moveToPos(first);

		while (first < last) {
			while (first < last && moveToPos(last).expo >= key.expo) {
				last--;
			}
			moveToPos(first) = moveToPos(last);
			while (first < last && moveToPos(first).expo <= key.expo) {
				first++;
			}
			moveToPos(last) = moveToPos(first);
		}

		moveToPos(first) = key;
		sortPolynomial(low, first - 1);
		sortPolynomial(first + 1, high);
	}

	// Reload operator + to complete the addition between two polynomials
	UnivariateSparsePolynomials& operator +(UnivariateSparsePolynomials& poly)
	{
		int totalLength = this->getLength() > poly.getLength() ?
			this->getLength() : poly.getLength();
		this->moveToStart(); poly.moveToStart();
		for (int i = 0; i < totalLength; i++) {
			if (poly.curr->expo < this->curr->expo) {
				this->insert(poly.curr);
				poly.next();
			} else if (poly.curr->expo == this->curr->expo) {
				this->curr->coef += poly.curr->coef;
			} else {
				this->next();
			}
		}
		return *this;
	}

	// Reload operator << to output the final polynomials
	friend std::ostream& operator <<(std::ostream& os, UnivariateSparsePolynomials& poly)
	{
		std::string plusSign = "", minusSign = " - ";
		for (int i = 0; i < poly.getLength(); i++) {
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
				plusSign = " + ";
			}
		}
		return os;
	}
};


int main()
{
	using namespace std;
	UnivariateSparsePolynomials poly_A, poly_B;
	cout << "Please input a term in the first polynomial: " << endl;
	poly_A.input();
	cout << "Please input a term in the second polynomial: " << endl;
	poly_B.input();
	cout << poly_A << endl << poly_B << endl;
	cout << poly_A + poly_B << endl;
	return 0;
}