#ifndef _TWO_WAY_LIST_H
#define _TWO_WAY_LIST_H

template <class Type>
class TwoWayList {

	public:

		// basic constructor function
		TwoWayList ();
		
		// deconstructor function
		~TwoWayList ();

		// swap operator 
		void operator &= (TwoWayList & List);

		// add to current pointer position
		void Insert (Type *Item);

		// remove from current position
		void Remove (Type *Item);

		// get a reference to the current item, plus the offset given
		Type* Current (int offset);

		// move the current pointer position backward through the list
		void Retreat ();

		// move the current pointer position forward through the list
		void Advance ();

		// operations to check the size of both sides
		int LeftLength ();
		int RightLength ();

		// operations to swap the left and right sides of two lists
		void SwapLefts (TwoWayList & List); 
		void SwapRights (TwoWayList & List); 

		// operations to move the the start of end of a list
		void MoveToStart ();
		void MoveToFinish (); 

		TwoWayList (TwoWayList & List);
	private:

		struct Node {
			// data
			Type *data;
			Node *next;
			Node *previous;
			
			// constructor
			Node () : data (0), next (0), previous (0) {}

			// deconstructor 
			~Node () 
			{
				delete data;
			}
		};


		struct Header {
			// data
			Node * first;
			Node * last;
			Node * current;
			int leftSize;
			int rightSize;
		};

		// the list itself is pointed to by this pointer	
		Header *list;	

};

#endif
