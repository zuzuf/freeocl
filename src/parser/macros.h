/*
	FreeOCL - a free OpenCL implementation for CPU
	Copyright (C) 2011  Roland Brochard

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>
*/
#ifndef __FREEOCL_PARSER_MACROS_H__
#define __FREEOCL_PARSER_MACROS_H__

#include "node.h"

#define BEGIN()		const size_t start = processed.size();	size_t __max = 0;	bool bOk = false
#define ROLLBACK()	rollBackTo(start)
#define END()		do\
					{\
						ROLLBACK();\
						return 0;\
					} while(false)
#define CHECK(N, MSG)	if (__max >= N)\
					{\
						error(MSG);\
						throw MSG;\
					}
#define MAX(X)		__max = std::max<size_t>(__max, X)

#define RUN()		if (!bOk)\
						ROLLBACK();\
					else

#define MATCH1(A)		if (__##A())	bOk = true;\
						RUN()

#define MATCH2(A,B)		if (__##A())\
						{\
							const Node N1 = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								d_val__ = Node(N1,d_val__);\
								bOk = true;\
							}\
						}\
						RUN()

#define MATCH3(A,B,C)	if (__##A())\
						{\
							const Node N1 = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								const Node N2 = d_val__;\
								MAX(2);\
								if (__##C())\
								{\
									d_val__ = Node(N1,N2,d_val__);\
									bOk = true;\
								}\
							}\
						}\
						RUN()

#define MATCH4(A,B,C,D)	if (__##A())\
						{\
							const Node N1 = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								const Node N2 = d_val__;\
								MAX(2);\
								if (__##C())\
								{\
									const Node N3 = d_val__;\
									MAX(3);\
									if (__##D())\
									{\
										d_val__ = Node(N1,N2,N3,d_val__);\
										bOk = true;\
									}\
								}\
							}\
						}\
						RUN()

#define MATCH5(A,B,C,D,E)	if (__##A())\
							{\
								const Node N1 = d_val__;\
								MAX(1);\
								if (__##B())\
								{\
									const Node N2 = d_val__;\
									MAX(2);\
									if (__##C())\
									{\
										const Node N3 = d_val__;\
										MAX(3);\
										if (__##D())\
										{\
											const Node N4 = d_val__;\
											MAX(4);\
											if (__##E())\
											{\
												d_val__ = Node(N1,N2,N3,N4,d_val__);\
												bOk = true;\
											}\
										}\
									}\
								}\
							}\
							RUN()

#define MATCH6(A,B,C,D,E,F)	if (__##A())\
							{\
								const Node N1 = d_val__;\
								MAX(1);\
								if (__##B())\
								{\
									const Node N2 = d_val__;\
									MAX(2);\
									if (__##C())\
									{\
										const Node N3 = d_val__;\
										MAX(3);\
										if (__##D())\
										{\
											const Node N4 = d_val__;\
											MAX(4);\
											if (__##E())\
											{\
												const Node N5 = d_val__;\
												MAX(5);\
												if (__##F())\
												{\
													d_val__ = Node(N1,N2,N3,N4,N5,d_val__);\
													bOk = true;\
												}\
											}\
										}\
									}\
								}\
							}\
							RUN()

#define MATCH7(A,B,C,D,E,F,G)	if (__##A())\
							{\
								const Node N1 = d_val__;\
								MAX(1);\
								if (__##B())\
								{\
									const Node N2 = d_val__;\
									MAX(2);\
									if (__##C())\
									{\
										const Node N3 = d_val__;\
										MAX(3);\
										if (__##D())\
										{\
											const Node N4 = d_val__;\
											MAX(4);\
											if (__##E())\
											{\
												const Node N5 = d_val__;\
												MAX(5);\
												if (__##F())\
												{\
													const Node N6 = d_val__;\
													MAX(6);\
													if (__##G())\
													{\
														d_val__ = Node(N1,N2,N3,N4,N5,N6,d_val__);\
														bOk = true;\
													}\
												}\
											}\
										}\
									}\
								}\
							}\
							RUN()

#define LISTOF_LEFT(A)	if (__##A())\
						{\
							Node N = d_val__;\
							while (__##A())\
								N = Node(N, d_val__);\
							d_val__ = N;\
							return 1;\
						}

#define LISTOF_RIGHT(A)	if (__##A())\
						{\
							std::deque<Node> stack;\
							stack.push_back(d_val__);\
							while (__##A())\
								stack.push_back(d_val__);\
							while (stack.size() > 1)\
							{\
								const Node N0 = stack.back();\
								stack.pop_back();\
								const Node N1 = stack.back();\
								stack.pop_back();\
								stack.push_back(Node(N1, N0));\
							}\
							d_val__ = stack.back();\
							return 1;\
						}

#define LISTOF_LEFT_SEP(A, SEP)\
						if (__##A())\
						{\
							Node N = d_val__;\
							size_t l = processed.size();\
							while (__##SEP())\
							{\
								const Node N1 = d_val__;\
								if (!__##A())\
								{\
									rollBackTo(l);\
									break;\
								}\
								N = Node(N, N1, d_val__);\
								l = processed.size();\
							}\
							d_val__ = N;\
							return 1;\
						}

#define LISTOF_RIGHT_SEP(A, SEP)\
						if (__##A())\
						{\
							std::deque<Node> stack;\
							stack.push_back(d_val__);\
							size_t l = processed.size();\
							while (__##SEP())\
							{\
								const Node N = stack.back();\
								stack.pop_back();\
								stack.push_back(Node(N, d_val__));\
								if (!__##A())\
								{\
									rollBackTo(l);\
									break;\
								}\
								stack.push_back(d_val__);\
								l = processed.size();\
							}\
							while (stack.size() > 1)\
							{\
								const Node N0 = stack.back();\
								stack.pop_back();\
								const Node N1 = stack.back();\
								stack.pop_back();\
								stack.push_back(Node(N1, N0));\
							}\
							d_val__ = stack.back();\
							return 1;\
						}

#define RULE1(A)				MATCH1(A)				return 1
#define RULE2(A,B)				MATCH2(A,B)				return 1
#define RULE3(A,B,C)			MATCH3(A,B,C)			return 1
#define RULE4(A,B,C,D)			MATCH4(A,B,C,D)			return 1
#define RULE5(A,B,C,D,E)		MATCH5(A,B,C,D,E)		return 1
#define RULE6(A,B,C,D,E,F)		MATCH6(A,B,C,D,E,F)		return 1
#define RULE7(A,B,C,D,E,F,G)	MATCH7(A,B,C,D,E,F,G)	return 1

#endif
