/*
Module : SortedArray.h
Purpose: Interface for an MFC template class which provides sorting and ordered insertion
         derived from the MFC class CArray or ATL class CSimpleArray
Created: PJN / 25-12-1999
History: PJN / 12-01-2000 Fixed a stack overflow in CSortedArray::Sort
         PJN / 21-02-2000 Fixed a number of problems in CSortedArray::Find
         PJN / 22-02-2000 Fixed a problem in CSortedArray::Find when there are no items in the array
         PJN / 29-02-2000 Fixed a problem in CSortedArray::Sort when there are no items in the array
         PJN / 27-08-2000 1. Fixed another stack overflow problem in CSortedArray::Sort.
                          2. Fixed a problem in CSortedArray::Sort where the comparison function
                          was returning negative values, 0 and positive values instead of -1, 0 & 1.
                          Thanks to Ted Crow for finding both of these problems.
         PJN / 01-10-2001 Fixed another bug in Sort!. Thanks to Jim Johnson for spotting this.
         PJN / 29-05-2002 1. Fixed a problem in CSortedArray::OrderedInsert. Thanks to John Young
                          for spotting and fixing this problem.
                          2. Updated copyright and usage instructions
         PJN / 06-12-2002 1. Rewrote the Sort method following reports of further problems by 
                          Serhiy Pavlov and Brian Rhodes
         PJN / 11-12-2002 1. Optimized code by replacing all calls to CArray<>::ElementAt with CArray<>::GetData
         PJN / 24-01-2003 1. Made CSortedArray::Find method const. Thanks to Serhiy Pavlov for reporting this.
         PJN / 18-08-2003 1. Made the class optionally independent of MFC. If the class detects than MFC is 
                          not being included, then the code will use CSimpleArray instead of CArray. This is 
                          a class provided in ATL as a substitute for CArray. Please note that the function
                          "OrderedInsert" is not available when using CSimpleArray as the parent class as 
                          CSimpleArray does not implement an "InsertAt" method.
         PJN / 03-11-2003 1. Now includes a new class "CSimpleArrayEx" which provides InsertAt support
                          for ATL's CSimpleArray class. This is now used by CSortedArray, rather than directly
                          using CSimpleArray
         PJN / 16-10-2004 1. Class now compiles cleanly on VC 7 if "Detect 64 bit portability issues" is enabled
                          as well as "Force conformance in for loops" is enabled.
         PJN / 22-12-2004 1. ASSERT / ATLASSERT and INT_PTR / int typedefs are now all done in one place. Thanks
                          to Serhiy Pavlov for suggesting this.
                          2. All functions are now declared in the class declaration
                          3. Reworked the classes to break the actual comparison code into a new traits class. 
                          You now have the choice of using a traits class which specifies the comparison function
                          via a function (CSortedArrayCompareFunction) or via a functor (CSortedArrayCompareFunctor).
                          Backward compatibility is kept by defined a class called CSortedArray which uses
                          a traits class which uses a function. If you want to use the new faster functor version
                          of the class then simply replace all instances of CSortedArray with CSortedArrayEx. 
                          Thanks to Serhiy Pavlov for this really nice addition.
                          4. Made CSortedArray::Find method non const again to allow use of GetData function. 
                          5. Updated the sample app to perform some speed tests on ATL Vs MFC and function 
                          pointer Vs Functor implementations.
         PJN / 11-10-2005 1. Updated the Find function to allow <0, 0 and >0 values to be allowed for the return
                          value from the comparison function / functor. This allows CString::Compare to be easily
                          used for comparison. Thanks to Serhiy Pavlov for reporting this.
                          2. Removed unused constructor from CSimpleArrayEx class.
                          3. Updated copyright details.
         PJN / 07-07-2006 1. Updated copyright details.
                          2. Minor update to the sample app to allow it to clean compile on VC 2005.
                          3. Updated the documentation to use the same style as the web site.
         PJN / 29-07-2006 1. Provided a new UniqueSort method which in addition to performing the standard sorting
                          of the array also removes any duplicates found. Thanks to John Cullen for suggesting this 
                          new feature.
         PJN / 29-06-2008 1. Updated copyright details
                          2. Code now compiles cleanly using Code Analysis (/analyze)
                          3. Updated the sample app to clean compile on VC 2008
                          4. The code now only supports VC 2005 or later.
         PJN / 26-07-2009 1. The code now natively uses INT_PTR for the index values
                          2. Updated the sample app's project settings to more modern default values.
                          3. If the code is compiled in ATL mode only, CSortedArrayBase (and ultimately CSortedArray/Ex)
                          are now derived from the ATL class CAtlArray instead of the author's CSimpleArrayEx class.
                          The CSimpleArrayEx class is now not included in the download and should be considered defunct. 
                          Thanks to Anatoly Ivasyuk for prompting this update.
                          4. Reordered the template parameters for CSortedArrayEx to use a default parameter 
                          for ARG_TYPE = const TYPE&. You will need to change the ordering of the template parameters 
                          in any client code which uses CSortedArrayEx.
         PJN / 11-08-2009 1. Following testing of the Sort method to ensure correctness, this method has been completely
                          reimplemented. When using the functor version of this method, it is now nearly 20% faster compared
                          to the previous version as well as addressing some sorting errors for specific arrays.
                          2. Fixed a bug in UniqueSort where it incorrectly used the pointer returned by GetData when calling
                          the comparison function. If you called UniqueSort with a nHighIndex < GetUpperBound() for the array
                          then the value returned by GetData could become corrupt if the array was realloc'ed.
                          3. Addition of an IsSorted simple helper method
                          4. Updated the code in the test app to better exercise the functionality of the class
                          5. Please note that since the implementation of the Sort method is implemented recursively, you can
                          run out of Win32 stack space if you use the code to sort extra large sized arrays. Some informal
                          testing indicates that with a standard Win32 1 MB stack, you will hit a stack overflow with an 
                          array containing random values from 0 to 1000 at roughly 2.9 million elements. Bear in mind that the 
                          amount of stack space used will depend on the actual values and their positions in your arrays. You 
                          will need to be aware of this issue if you will be using the code for array sizes upwards of a few 
                          hundred thousand. I may consider reimplementing the code to avoid using the Win32 stack to implement 
                          the recursion if anyone things this would be a useful addition.
                          6. Addition of a StableSort method. Unlike the "Sort" method which internally uses the quicksort 
                          algorithm which is not stable, StableSort internally uses an insertion sort algorithm which is. 
                          Thanks to "yv" for prompting this update.
         PJN / 07-09-2009 1. OrderedInsert and Find methods are now Non-const methods. This allows the code to call the non const
                          versions of CArray::GetData which helps avoids compiler errors in some client scenarios.
         PJN / 11-07-2010 1. Updated copyright details
                          2. Updated sample app to compile cleanly on VC 2010
                          3. Optimized code in the Sort method which remembers the "key" element while the quicksort is being
                          performed. Thanks to Michael Stephenson for reporting this optimization.
         PJN / 06-11-2010 1. Sort method now internally uses std::sort for sorting. This leads to dramatic improvements as the size
                          of the array increases. It also means that issues with stack sizes due to recursion are now gone. Here is
                          some before and after figures in ms for sorting an array of integers as obtained from the sample app (Note 
                          please do not compare the absolute values from one row to another as I shrunk down the number of array 
                          loops to keep the measured times reasonable as the array element size increased):

                          Elements  Before (Function pointer array)  Before (Functor)   After (Function Pointer array)  After (Functor)
                          100       34                               7                  16 (x2.125)                     6 (x1.16)
                          1000      517                              176                295 (x1.75)                     84 (x2.09)
                          10000     7896                             2398               3525 (x2.23)                    1098 (x2.18)
                          100000    2696                             529                336 (x8.03)                     97 (x5.45)
                          1000000   21017                            3284               378 (x55)                       125 (x26)
                          10000000  208768                           30605              899 (x232)                      458 (x66)

                          I believe the reason we see a dramatic improvement in performance as the array size increases is the fact that 
                          std::sort uses an introsort algorithm (which is a quicksort which switches to a heapsort when the recursion reaches
                          a certain depth). The more expert C++ developers out there may ask why not just use the standard STL collection
                          classes instead of the old style MFC CArray classes. In my case, many of my classes are pure MFC classes and at the 
                          time of their initial development the MFC classes were the number one choice. Now if you are writing new code it
                          really does make sense to use the STL classes but it is still nice to have the familiarity of the MFC collection
                          classes with the performance of their STL brethren.
                          2. StableSort method now internally uses std::stable_sort. Again this has lead to pretty substantial performance 
                          improvements as the size of the array increases (Again note please do not compare the absolute values from one row 
                          to another as I shrunk down the number of array loops to keep the measured times reasonable as the array element 
                          size increased):
                                                  
                          Elements Before (Function pointer array)  Before (Functor)   After (Function Pointer array)  After (Functor)
                          100      249                              80                 246 (x1.01)                     109 (x0.773)
                          1000     1005                             274                275 (x3.65)                     120 (x2.28)
                          10000    913                              229                45  (x20.29)                    13 (x17.61)
                          50000    22587                            5655               172 (x131)                      74 (x76)
                          100000   90484                            22683              379 (x238)                      154 (x147)
                          300000   81606                            20420              111 (x735)                      48 (x425)
         PJN / 16-03-2012 1. Updated copyright details.
                          2. Addition of a InsertionBehaviour parameter to the OrderedInsert method. This new enum allows you to specify what
                          should happen if a duplicate item is found at the tentative insertion point. This new enum can have the values:
                          AllowDuplicate (which is the normal behavior), OverwriteIfDuplicate, LeaveIfDuplicate & FailIfDuplicate. Thanks to 
                          Michael Stephenson for providing this nice addition.
         PJN / 20-04-2012 1. Addition of a FindOrderedInsertIndex method. This new method returns the index which an item would be inserted at
                          if OrderedInsert was called with the same element without actually inserting the item. Thanks to Michael Stephenson 
                          for providing this nice addition.

Copyright (c) 1999 - 2012 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////// Macros / Includes /////////////////////////////////

#pragma once

#ifndef __SORTEDARRAY_H__
#define __SORTEDARRAY_H__

#ifdef SORTEDARRAY_ATL_ONLY
  #ifndef __ATLCOLL_H__
    #include <atlcoll.h>
    #pragma message("To avoid this message, please put atlcoll.h in your pre compiled header (usually stdafx.h)")
  #endif
#else
  #ifndef __AFXTEMPL_H__
    #include <afxtempl.h> 
    #pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (usually stdafx.h)")
  #endif
#endif

#include <algorithm>


/////////////////////////// Classes ///////////////////////////////////////////

//Class which provides a compare function via a class declaration. Used
//when deriving from CSortedArrayBase. It is a so called traits class
template<class TYPE, class ARG_TYPE = const TYPE&>
class CSortedArrayCompareFunction
{
public:
//Typedefs
  typedef int COMPARE_FUNCTION(ARG_TYPE element1, ARG_TYPE element2);
  typedef COMPARE_FUNCTION* LPCOMPARE_FUNCTION;

//Constructors / Destructors
  CSortedArrayCompareFunction() : m_pCompareFunction(NULL)
  {
  }

//Methods
  void SetCompareFunction(LPCOMPARE_FUNCTION pCompareFunction)
  {
    ATLASSERT(pCompareFunction);
    m_pCompareFunction = pCompareFunction;
  }

  LPCOMPARE_FUNCTION GetCompareFunction() const 
  { 
    return m_pCompareFunction; 
  }

#ifdef _DEBUG
  BOOL IsCompareFunctionValid() const 
  { 
    return (m_pCompareFunction != NULL); 
  }
#endif

protected:
//Member variables
  LPCOMPARE_FUNCTION m_pCompareFunction;
};

//Class which provides a compare function via a functor. Used
//when deriving from CSortedArrayBase. It is a so called traits class
template<class COMPARE_TYPE>
class CSortedArrayCompareFunctor
{
public:
//methods
  void SetCompareFunction(const COMPARE_TYPE& pCompareFunctor)
  {
    m_pCompareFunction = pCompareFunctor;
  }

  COMPARE_TYPE GetCompareFunction() const 
  { 
    return m_pCompareFunction; 
  }

#ifdef _DEBUG
  BOOL IsCompareFunctionValid() const 
  { 
    return TRUE; 
  }
#endif

protected:
//Member variables
  COMPARE_TYPE m_pCompareFunction;
};

//Class which contains the enum InsertionBehaviour define.
class CSortedArrayEnums
{
public:
//enums
  enum InsertionBehaviour
  {
    AllowDuplicate       = 0,
    OverwriteIfDuplicate = 1,
    LeaveIfDuplicate     = 2,
    FailIfDuplicate      = 3
  };
};

//Class used when we are using std::sort or std::stable_sort
//template<typename T, typename COMPARE_TYPE> 
template<typename ARG_TYPE, class COMPARE_TYPE>
class CSortedArrayCompareSTL
{
public:
//Methods  
  bool operator() (ARG_TYPE x, ARG_TYPE y) const
  {
    return m_comp.GetCompareFunction()(x, y) < 0; 
  }

//Member variables
  COMPARE_TYPE m_comp;
};

//Base class which implements sorting for its parent class CArray / CAtlArray
#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class COMPARE_TYPE, typename ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE> >
class CSortedArrayBase : public CAtlArray<TYPE, ETraits>, public COMPARE_TYPE
#else
template<class TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE& >
class CSortedArrayBase : public CArray<TYPE, ARG_TYPE>, public COMPARE_TYPE
#endif
{
public:
//Methods
#ifdef SORTEDARRAY_ATL_ONLY
  //To make up for the slight differences between CAtlArray and CArray
  INT_PTR GetSize() const { return GetCount(); };
  bool SetSize(size_t nNewSize, int nGrowBy = -1) { return SetCount(nNewSize, nGrowBy); };
#endif  

  INT_PTR OrderedInsert(ARG_TYPE element, INT_PTR nCount = 1, CSortedArrayEnums::InsertionBehaviour behavior = CSortedArrayEnums::AllowDuplicate)
  {
    //Validate our parameters
	  ATLASSERT(IsCompareFunctionValid());

	  INT_PTR nLowIndex = 0;
	  INT_PTR nHighIndex = GetSize() - 1;

    TYPE* pData = GetData();
	  
	  //Find the insert location (mid) for the new element.
	  int nResult = 0;
	  INT_PTR nMidIndex = nHighIndex / 2;
	  while (nHighIndex >= nLowIndex)
	  {
      ATLASSUME(pData); //Note we do not do the ATLASSERT outside of the loop because
                        //the value "pData" can be NULL in a fresh new array. But
                        //in this case we will not end up executing this code. Also
                        //since it is an ATLASSERT it will be completely optimized away
                        //in release mode builds so it has no impact on performance

		  nResult = m_pCompareFunction(element, pData[nMidIndex]);
		  if (!nResult)
			  break;

		  if (nResult < 0)
			  nHighIndex = nMidIndex - 1;	//Insert in the lower half
		  else 
        nLowIndex = nMidIndex + 1; //Insert in the upper half
		  nMidIndex = ((nHighIndex - nLowIndex) / 2) + nLowIndex;
	  }

    //Handle the behavior parameter when it is not "AllowDuplicate"
	  if (nResult == 0 && (behavior != CSortedArrayEnums::AllowDuplicate) && (GetSize() > 0))
	  {
	    //It doesn't make sense to call OrderedInsert with a behavior different to AllowDuplicate where nCount != 1
	    ATLASSERT(nCount == 1);
	  
	    if (behavior == CSortedArrayEnums::OverwriteIfDuplicate)
	    {
	      //Overwrite the element
	      GetAt(nMidIndex) = element;
	      return nMidIndex;
	    }
	    else if (behavior == CSortedArrayEnums::FailIfDuplicate)
	    {
	      //Fail because of a duplicate
	      return -1;
	    }
	    else
	    {
	      //Leave alone because of a duplicate
	      ATLASSERT(behavior == CSortedArrayEnums::LeaveIfDuplicate);
	      return nMidIndex;
	    }
	  }

    //Finally do the insert by delegating to "InsertAt"
	  InsertAt(nMidIndex, element, nCount);

	  return nMidIndex;
  }
  
  INT_PTR FindOrderedInsertIndex(ARG_TYPE element, CSortedArrayEnums::InsertionBehaviour behavior = CSortedArrayEnums::AllowDuplicate)
  {
    //Validate our parameters
	  ATLASSERT(IsCompareFunctionValid());

	  INT_PTR nLowIndex = 0;
	  INT_PTR nHighIndex = GetSize() - 1;

    TYPE* pData = GetData();

	  //Find the insert location (mid) for the new element.
	  int nResult = 0;
	  INT_PTR nMidIndex = nHighIndex / 2;
	  while (nHighIndex >= nLowIndex)
	  {
      ATLASSUME(pData); //Note we do not do the ATLASSERT outside of the loop because
                        //the value "pData" can be NULL in a fresh new array. But
                        //in this case we will not end up executing this code. Also
                        //since it is an ATLASSERT it will be completely optimized away
                        //in release mode builds so it has no impact on performance

		  nResult = m_pCompareFunction(element, pData[nMidIndex]);
		  if (!nResult)
			  break;

		  if (nResult < 0)
			  nHighIndex = nMidIndex - 1;	//Insert in the lower half
		  else 
        nLowIndex = nMidIndex + 1; //Insert in the upper half
		  nMidIndex = ((nHighIndex - nLowIndex) / 2) + nLowIndex;
	  }

    //Handle the behavior parameter when it is not "AllowDuplicate"
	  if (nResult == 0 && (behavior != CSortedArrayEnums::AllowDuplicate) && (GetSize() > 0))
	  {
	    if (behavior == CSortedArrayEnums::OverwriteIfDuplicate)
	    {
	      //Overwrite the element
	      return nMidIndex;
	    }
	    else if (behavior == CSortedArrayEnums::FailIfDuplicate)
	    {
	      //Fail because of a duplicate
	      return -1;
	    }
	    else
	    {
	      //Leave alone because of a duplicate
	      ATLASSERT(behavior == CSortedArrayEnums::LeaveIfDuplicate);
	      return nMidIndex;
	    }
	  }

	  return nMidIndex;
  }
  
  void StableSort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
  {
    //Sorting all the data is implied by setting nHighIndex = -1
    if (nHighIndex == -1)
      nHighIndex = GetSize() - 1;

    //Return if we have reached the base case of zero or one elements
    INT_PTR nLength = nHighIndex - nLowIndex + 1;
    if (nLength <= 1)
      return;               

    //Validate our parameters
    ATLASSERT(nLowIndex >= 0 && nLowIndex < GetSize());
    ATLASSERT(nHighIndex < GetSize());
    ATLASSERT(IsCompareFunctionValid());
    TYPE* pData = GetData();
    ATLASSUME(pData);

    //Use std::stable_sort to do the heavy lifting for us
    CSortedArrayCompareSTL<ARG_TYPE, COMPARE_TYPE> less;
    less.m_comp.SetCompareFunction(m_pCompareFunction);
    std::stable_sort(pData + nLowIndex, pData + nHighIndex + 1, less);
  }
  
  void Sort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
  {
    //Sorting all the data is implied by setting nHighIndex = -1
    if (nHighIndex == -1)
      nHighIndex = GetSize() - 1;

    //Return if we have reached the base case of zero or one elements
    INT_PTR nLength = nHighIndex - nLowIndex + 1;
    if (nLength <= 1)
      return;               

    //Validate our parameters
    ATLASSERT(nLowIndex >= 0 && nLowIndex < GetSize());
    ATLASSERT(nHighIndex < GetSize());
    ATLASSERT(IsCompareFunctionValid());
    TYPE* pData = GetData();
    ATLASSUME(pData);

    //Use std::sort to do the heavy lifting for us
    CSortedArrayCompareSTL<ARG_TYPE, COMPARE_TYPE> less;
    less.m_comp.SetCompareFunction(m_pCompareFunction);
    std::sort(pData + nLowIndex, pData + nHighIndex + 1, less);
  }
 
  void UniqueSort(INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
  {
    //Sorting all the data is implied by setting nHighIndex = -1
	  if (nHighIndex == -1)
      nHighIndex = GetSize() - 1;     
 
    //Return if we have reached the base case of zero or one elements
    INT_PTR nLength = nHighIndex - nLowIndex + 1;
    if (nLength <= 1)
      return;               

    //First sort the requested elements, using the "Sort" method
    Sort(nLowIndex, nHighIndex);
    
    //Now remove any duplicates
    for (INT_PTR i=nHighIndex; i>nLowIndex; i--)
    {
      if (m_pCompareFunction(operator[](i), operator[](i-1)) == 0)
        RemoveAt(i);
    }
  }

  INT_PTR Find(ARG_TYPE element, INT_PTR nLowIndex = 0, INT_PTR nHighIndex = -1)
  {
    //Validate our parameters
    ATLASSERT(IsCompareFunctionValid());

    INT_PTR nSize = GetSize();

    //If there are no items in the array, then return immediately
    if (nSize == 0)
      return -1;

    INT_PTR left = nLowIndex;
    INT_PTR right = nHighIndex;
    TYPE* pData = GetData();

    if (right == -1)
      right = nSize - 1;

    ATLASSERT(left <= right);
    ATLASSUME(pData);
  
    if (left == right) //Straight comparison fewer than 2 elements to search
      return (m_pCompareFunction(pData[left], element) == 0) ? left : -1;

    //do a binary chop to find the location where the element should be inserted
    INT_PTR nFoundIndex = -1;
    while ((nFoundIndex == -1) && (left != right))
    {
      INT_PTR nCompareIndex;

      if (right == (left + 2))
        nCompareIndex = left+1;
      else
        nCompareIndex = ((right - left) >> 1) + left;

      int nCompare = m_pCompareFunction(pData[nCompareIndex], element);

      if (nCompare < 0)
      {
        if ((right - left) == 1)
        {
          if (m_pCompareFunction(pData[right], element) == 0)
            nFoundIndex = right;
          else if (m_pCompareFunction(pData[left], element) == 0)
            nFoundIndex = left;
          else
            left = right;
        }
        else
          left = nCompareIndex;
      }
      else if (nCompare > 0)
      {
        if ((right - left) == 1)
        {
          if (m_pCompareFunction(pData[right], element) == 0)
            nFoundIndex = right;
          else if (m_pCompareFunction(pData[left], element) == 0)
            nFoundIndex = left;
          else
            right = left;
        }
        else
          right = nCompareIndex;
      }
      else
      {
        nFoundIndex = nCompareIndex;
      }
    }
  
    return nFoundIndex;
  }

  BOOL IsSorted() const 
  {
    //Validate our parameters
    ATLASSERT(IsCompareFunctionValid());
  
    //What will be the return value from this function (assume the best)
    BOOL bSorted = TRUE;
    
    //Validate the data is truly sorted
    INT_PTR nSize = GetSize();
    if (nSize >= 2) //By defintion an array with less than two items in it is sorted
    {
      const TYPE* pData = GetData();
      ATLASSUME(pData);
      for (INT_PTR i=1; i<nSize && bSorted; i++)
        bSorted = (m_pCompareFunction(pData[i-1], pData[i]) <= 0);
    }

    return bSorted;
  }
};

//A derivation of CSortedArrayBase which provides the comparison via a function pointer
#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE> >
class CSortedArray : public CSortedArrayBase<TYPE, CSortedArrayCompareFunction<TYPE, ARG_TYPE>, ARG_TYPE, ETraits >
#else
template<class TYPE, class ARG_TYPE = const TYPE&>
class CSortedArray : public CSortedArrayBase<TYPE, CSortedArrayCompareFunction<TYPE, ARG_TYPE>, ARG_TYPE >
#endif
{
};

//A derivation of CSortedArrayBase which provides the comparison via a functor
#ifdef SORTEDARRAY_ATL_ONLY
template<typename TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE&, class ETraits = CElementTraits<TYPE> >
class CSortedArrayEx : public CSortedArrayBase<TYPE, CSortedArrayCompareFunctor<COMPARE_TYPE>, ARG_TYPE, ETraits >
#else
template<class TYPE, class COMPARE_TYPE, class ARG_TYPE = const TYPE& >
class CSortedArrayEx : public CSortedArrayBase<TYPE, CSortedArrayCompareFunctor<COMPARE_TYPE>, ARG_TYPE >
#endif
{
};

#endif //__SORTEDARRAY_H__
