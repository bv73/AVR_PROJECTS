#define KBD_BUFF_SIZE (32)
#define TIMEOUT_KEYBOARD (2) // Оптимальное значение 2 мс

unsigned char bitcount;
unsigned char kb_buffer[KBD_BUFF_SIZE];
unsigned char *input_pointer;
unsigned char *output_pointer;
unsigned char buffcnt;
unsigned char kbd_data_byte; // байт для приема последов. данных с клавиатуры
unsigned char kbd_data_mask;
unsigned char is_up; // Флаг отпускания кнопки
unsigned char shift;  // Флаг нажатого шифта
unsigned char mode; // Флаг режима
unsigned char TimeOut_KBD;
