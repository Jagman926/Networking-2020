#include "a3_InputHandler.h"
#include <string>
#include <algorithm>

void InputIn(a3_InputHandler* input, int asciiIn)
{
	// Add ascii valie to buffer write location
	input->buffer[input->bufferWriteLoc] = asciiIn;
	// Increment write location
	input->bufferWriteLoc++;
}

void ClearLastInputValue(a3_InputHandler* input)
{
	if (input->bufferWriteLoc != 0)
	{
		// Decrement write location
		input->bufferWriteLoc--;
		// Set value to null
		input->buffer[input->bufferWriteLoc] = 0;
	}
}

void ClearInputBuffer(a3_InputHandler* input)
{
	// Copy current input buffer to last buffer
	strcpy(input->lastInputBuffer, input->buffer);
	// Clear current input buffer
	memset(input->buffer, 0, sizeof input->buffer);
	// Reset buffer write to start
	input->bufferWriteLoc = 0;
}

void ClearLastInputBuffer(a3_InputHandler* input)
{
	// Clear lastInputBuffer
	memset(input->lastInputBuffer, 0, sizeof input->lastInputBuffer);
}
