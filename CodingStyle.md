# Coding style #

  * Encoding : UTF-8
  * Tabs : real tabs, no spaces
  * Spaces around binary operators
  * Line size limit : 120 chars
  * Code documentation : Doxygen style
  * Macro and define names : in upper case
  * Naming convention : everything\_in\_lower\_case\_with\_underscores except the FreeOCL namespace.

# Small sample #
```
/** \brief function returning 0
  * \return 0
  */
int function_name()
{
	for(size_t i = 0 ; i < 10 ; ++i)
	{
		if (i == 9)
			break;
	}
	// Internal comment
	return 0 * (1 + 0);
}
```