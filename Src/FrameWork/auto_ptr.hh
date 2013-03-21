/*
The contents of this file are subject to the NOKOS License Version 1.0
(the "License"); you may not use this file except in compliance with the
License.

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF  ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Original Software is
TVT-tools.

Copyright © 2001 Nokia and others. All Rights Reserved.

Contributor(s): Heikki Virtanen.
*/

// FILE_DES: auto_ptr.hh: Auxiliary
// Heikki Virtanen

// $Id: auto_ptr.hh 1.4 Fri, 27 Feb 2004 16:36:58 +0200 warp $
// 
// auto_ptr is pointer wrapper, which can be used to own dynamically
// allocated memory and to release it automatically.

// $Log: auto_ptr.hh,v $
// Revision 1.1  1999/07/13 15:24:32  hvi
// Initial revision
//


#ifndef TOOLS_AUTO_PTR_HH
#define TOOLS_AUTO_PTR_HH

//#define explicit

#ifndef HAS_MEMBER_TEMPLATES
#define HAS_MEMBER_TEMPLATES 1
#endif

template<class X>
class auto_ptr
{

  X* px;

public:

  explicit auto_ptr(X* p=0): px(p) {}

#if HAS_MEMBER_TEMPLATES

  template<class Y>
  auto_ptr(auto_ptr<Y>& r) : px(r.release()) {}

  template<class Y>
  auto_ptr& operator=(auto_ptr& r)
  {
    reset(r.release());
    return *this;
  }

#else

  auto_ptr(auto_ptr& r) : px(r.release()) {}

  auto_ptr& operator=(auto_ptr& r)
  {
    reset(r.release());
    return *this;
  }

#endif

  ~auto_ptr()             { delete px; }

  X& operator*()    const { return *px; }
  X* operator->()   const { return px; }
  X* get()          const { return px; }
  X* release()            { X* p=px; px=0; return p; }
  void reset(X* p=0)      { if (px != p) delete px, px = p; }
};
#endif
