#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

//-----------------------------------------------------------------------------

#ifdef __cplusplus
extern "C"
{
#else	// !__cplusplus
typedef struct a3_InputHandler				a3_InputHandler;
#endif	// __cplusplus

//-----------------------------------------------------------------------------

	// input handler
	struct a3_InputHandler
	{
		// Input container
		char buffer[512];
		char lastInputBuffer[512];
		int bufferWriteLoc;
	};

//-----------------------------------------------------------------------------

	// adds an ascii value to the input buffer
	// Params
	//	param1 (int): ascii value being put into buffer
	void InputIn(a3_InputHandler* input, int asciiValue);

	// removes value from last buffer index
	void ClearLastInputValue(a3_InputHandler* input);

	// clears lastInputBuffer buffer
	void ClearLastInputBuffer(a3_InputHandler* input);

	// clears current input buffer
	void ClearInputBuffer(a3_InputHandler* input);

//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif // !INPUT_HANDLER_H

