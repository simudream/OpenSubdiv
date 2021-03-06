..  
     Copyright 2013 Pixar
  
     Licensed under the Apache License, Version 2.0 (the "Apache License")
     with the following modification; you may not use this file except in
     compliance with the Apache License and the following modification to it:
     Section 6. Trademarks. is deleted and replaced with:
  
     6. Trademarks. This License does not grant permission to use the trade
        names, trademarks, service marks, or product names of the Licensor
        and its affiliates, except as required to comply with Section 4(c) of
        the License and to reproduce the content of the NOTICE file.
  
     You may obtain a copy of the Apache License at
  
         http://www.apache.org/licenses/LICENSE-2.0
  
     Unless required by applicable law or agreed to in writing, software
     distributed under the Apache License with the above modification is
     distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
     KIND, either express or implied. See the Apache License for the specific
     language governing permissions and limitations under the Apache License.
  

glEvalLimit
-----------

.. contents::
   :local:
   :backlinks: none

SYNOPSIS
========

.. parsed-literal:: 
   :class: codefhead

   **limitEval** [**-f**] *objfile(s)*

DESCRIPTION
===========

``glEvalLimit`` is a stand-alone application that showcases the limit surface
Eval module. On the given shape, random samples are generated in local u,v space.
Vertex, varying and face-varying data is then computed on the surface limit and
displayed as colors. Multiple controls are available to experiment with the algorithms.

.. image:: images/evalLimit_hedit0.jpg 
   :width: 400px
   :align: center
   :target: images/evalLimit_hedit0.jpg 

OPTIONS
=======

**-f**
  Launches the application in full-screen mode (if is supported by GLFW on the
  OS)

.. include:: examples_see_also.rst
