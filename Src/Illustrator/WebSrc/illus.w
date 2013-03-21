%-*-mode: nuweb; nuweb-source-mode: "c++"; -*-
%%% $Source: Src/Illustrator/WebSrc/illus.w $
%%% $Id: illus.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi $

\documentclass[a4paper]{report}
\usepackage{times}
\usepackage[T1]{fontenc}
\usepackage[english]{babel}
\usepackage{latexsym}
\usepackage[dvips]{graphicx}
\usepackage{illus}


\title{ALA tools implementation\\
  \textsf{\large Drawing Labelled Transitions Systems Nicely}}
\author{Heikki Virtanen}
%$Format: "\\date{$Id: 34_illus.w,v 1.2 2000/08/21 10:29:29 hvi Exp $}"$
\date{illus.w 1.1 Mon, 25 Aug 2003 15:49:29 +0300 hvi}
\begin{document}

%%%%1
\maketitle
\begin{abstract}
This system is part of \emph{ALA} verification tool set (see
\emph{The ALA Verification Tool} \cite{ALA:tool} for more
information on \emph{ALA}). The ALA produces yes/no answers to well
formed questions or some kind of finite automaton which describes
behaviour of system verified. This tool produces pictures of those
finite automaton called \emph{labelled transitions systems} or
\emph{LTS} for humans to analyze.
\end{abstract}

%%%%2
\tableofcontents

%%%%3
\include{intro}

%%%%%%%
%%%%%%%
%%%%%%%

@i algor.w

@i gui.w

@i tools.w

@i compile.w

\bibliography{illus,geom}
\bibliographystyle{alpha}

\appendix


\chapter{Indexes}


\section{Files}

@f

\section{Macros}

@m

\section{Variables}

@u


\end{document}
