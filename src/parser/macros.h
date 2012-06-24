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

#define ERROR(MSG)	do { error(MSG); throw MSG; } while(false)
#define BEGIN(K)		const size_t start = processed.size();	size_t __max = 0;	bool b_ok = false;	smartptr<node> N[K];
#define BEGIN_SMALL()		const size_t start = processed.size();
#define ROLLBACK()	roll_back_to(start)
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

#define RUN()		if (!b_ok)\
						ROLLBACK();\
					else for(size_t start = processed.size(), end = 0 ; end == 0 ; ++end)

#define MATCH1(A)		b_ok = false;\
						if (__##A())	{	N[0] = d_val__; b_ok = true;	}\
						RUN()

#define MATCH2(A,B)		b_ok = false;\
						if (__##A())\
						{\
							N[0] = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								N[1] = d_val__;\
								b_ok = true;\
							}\
						}\
						RUN()

#define MATCH3(A,B,C)	b_ok = false;\
						if (__##A())\
						{\
							N[0] = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								N[1] = d_val__;\
								MAX(2);\
								if (__##C())\
								{\
									N[2] = d_val__;\
									b_ok = true;\
								}\
							}\
						}\
						RUN()

#define MATCH4(A,B,C,D)	b_ok = false;\
						if (__##A())\
						{\
							N[0] = d_val__;\
							MAX(1);\
							if (__##B())\
							{\
								N[1] = d_val__;\
								MAX(2);\
								if (__##C())\
								{\
									N[2] = d_val__;\
									MAX(3);\
									if (__##D())\
									{\
										N[3] = d_val__;\
										b_ok = true;\
									}\
								}\
							}\
						}\
						RUN()

#define MATCH5(A,B,C,D,E)	b_ok = false;\
							if (__##A())\
							{\
								N[0] = d_val__;\
								MAX(1);\
								if (__##B())\
								{\
									N[1] = d_val__;\
									MAX(2);\
									if (__##C())\
									{\
										N[2] = d_val__;\
										MAX(3);\
										if (__##D())\
										{\
											N[3] = d_val__;\
											MAX(4);\
											if (__##E())\
											{\
												N[4] = d_val__;\
												b_ok = true;\
											}\
										}\
									}\
								}\
							}\
							RUN()

#define MATCH6(A,B,C,D,E,F)	b_ok = false;\
							if (__##A())\
							{\
								N[0] = d_val__;\
								MAX(1);\
								if (__##B())\
								{\
									N[1] = d_val__;\
									MAX(2);\
									if (__##C())\
									{\
										N[2] = d_val__;\
										MAX(3);\
										if (__##D())\
										{\
											N[3] = d_val__;\
											MAX(4);\
											if (__##E())\
											{\
												N[4] = d_val__;\
												MAX(5);\
												if (__##F())\
												{\
													N[5] = d_val__;\
													b_ok = true;\
												}\
											}\
										}\
									}\
								}\
							}\
							RUN()

#define MATCH7(A,B,C,D,E,F,G)	b_ok = false;\
								if (__##A())\
								{\
									N[0] = d_val__;\
									MAX(1);\
									if (__##B())\
									{\
										N[1] = d_val__;\
										MAX(2);\
										if (__##C())\
										{\
											N[2] = d_val__;\
											MAX(3);\
											if (__##D())\
											{\
												N[3] = d_val__;\
												MAX(4);\
												if (__##E())\
												{\
													N[4] = d_val__;\
													MAX(5);\
													if (__##F())\
													{\
														N[5] = d_val__;\
														MAX(6);\
														if (__##G())\
														{\
															N[6] = d_val__;\
															b_ok = true;\
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
							smartptr<node> N = d_val__;\
							while (__##A())\
								N = new chunk(N, d_val__);\
							d_val__ = N;\
							return 1;\
						}

#define LISTOF_RIGHT(A)	if (__##A())\
						{\
							std::deque<smartptr<node> > stack;\
							stack.push_back(d_val__);\
							while (__##A())\
								stack.push_back(d_val__);\
							while (stack.size() > 1)\
							{\
								const smartptr<node> N0 = stack.back();\
								stack.pop_back();\
								const smartptr<node> N1 = stack.back();\
								stack.pop_back();\
								stack.push_back(new chunk(N1, N0));\
							}\
							d_val__ = stack.back();\
							return 1;\
						}

#define LISTOF_LEFT_SEP(A, SEP)\
						if (__##A())\
						{\
							smartptr<node> N = d_val__;\
							size_t l = processed.size();\
							while (__##SEP())\
							{\
								const smartptr<node> N1 = d_val__;\
								if (!__##A())\
								{\
									roll_back_to(l);\
									break;\
								}\
								N = new chunk(N, N1, d_val__);\
								l = processed.size();\
							}\
							d_val__ = N;\
							return 1;\
						}

#define LISTOF_LEFT_OP(A, SEP)\
						if (__##A())\
						{\
							smartptr<expression> N = d_val__;\
							size_t l = processed.size();\
							while (__##SEP())\
							{\
								int op = d_val__.as<token>()->get_id();\
								if (!__##A())\
								{\
									roll_back_to(l);\
									break;\
								}\
								N = new binary(op, N, d_val__);\
								l = processed.size();\
							}\
							d_val__ = N;\
							return 1;\
						}

#define LISTOF_RIGHT_SEP(A, SEP)\
						if (__##A())\
						{\
							std::deque<smartptr<node> > stack;\
							stack.push_back(d_val__);\
							size_t l = processed.size();\
							while (__##SEP())\
							{\
								const smartptr<node> N = stack.back();\
								stack.pop_back();\
								stack.push_back(new chunk(N, d_val__));\
								if (!__##A())\
								{\
									roll_back_to(l);\
									break;\
								}\
								stack.push_back(d_val__);\
								l = processed.size();\
							}\
							while (stack.size() > 1)\
							{\
								const smartptr<node> N0 = stack.back();\
								stack.pop_back();\
								const smartptr<node> N1 = stack.back();\
								stack.pop_back();\
								stack.push_back(new chunk(N1, N0));\
							}\
							d_val__ = stack.back();\
							return 1;\
						}

#define RULE1(A)				MATCH1(A)				return 1
#define RULE2(A,B)				MATCH2(A,B)\
								do {\
									d_val__ = new chunk(N[0], N[1]);\
									return 1;\
								} while(false)
#define RULE3(A,B,C)			MATCH3(A,B,C)\
								do {\
									d_val__ = new chunk(N[0], N[1], N[2]);\
									return 1;\
								} while(false)
#define RULE4(A,B,C,D)			MATCH4(A,B,C,D)\
								do {\
									d_val__ = new chunk(N[0], N[1], N[2], N[3]);\
									return 1;\
								} while(false)
#define RULE5(A,B,C,D,E)		MATCH5(A,B,C,D,E)\
								do {\
									d_val__ = new chunk(N[0], N[1], N[2], N[3], N[4]);\
									return 1;\
								} while(false)
#define RULE6(A,B,C,D,E,F)		MATCH6(A,B,C,D,E,F)\
								do {\
									d_val__ = new chunk(N[0], N[1], N[2], N[3], N[4], N[5]);\
									return 1;\
								} while(false)
#define RULE7(A,B,C,D,E,F,G)	MATCH7(A,B,C,D,E,F,G)\
								do {\
									d_val__ = new chunk(N[0], N[1], N[2], N[3], N[4], N[5], N[6]);\
									return 1;\
								} while(false)

#endif
