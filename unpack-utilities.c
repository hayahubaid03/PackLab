// Utilities for unpacking files
// PackLab - CS213 - Northwestern University

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unpack-utilities.h"


// --- public functions ---

void error_and_exit(const char* message) {
  fprintf(stderr, "%s", message);
  exit(1);
}

void* malloc_and_check(size_t size) {
  void* pointer = malloc(size);
  if (pointer == NULL) {
    error_and_exit("ERROR: malloc failed\n");
  }
  return pointer;
}

void parse_header(uint8_t* input_data, size_t input_len, packlab_config_t* config) {
  //checks against minimum length
  int init_len = 4;
  if (input_len < init_len) {
    config->is_valid = false;
    return;
  }

  //checks magic nums and version
  if (input_data[0] == 0x02 && input_data[1] == 0x13 && input_data[2] == 0x02){
    config -> is_valid = true;
  }
  else{
    config -> is_valid = false;
    return;
  }

  //check flags
 uint8_t flag = input_data[3];
  config->is_checksummed = (flag & 0x20) == 0x20;
  config->is_encrypted = (flag & 0x40) == 0x40;
  config->is_compressed = (flag & 0x80) == 0x80;


  if (config->is_compressed) {
    init_len += 16;
    for (size_t i = 4; i < init_len; i++) {
        config->dictionary_data[i-4] = input_data[i];
    }
  }

  if(config -> is_checksummed){
    uint16_t check_val = input_data[init_len];
    check_val = check_val << 8; 
    check_val += input_data[init_len + 1];
    config -> checksum_value = check_val;
    init_len += 2;
  }

  config -> header_len = init_len;
  }
 
  // TODO
  // Validate the header and set configurations based on it
  // Look at unpack-utilities.h to see what the fields of config are
  // Set the is_valid field of config to false if the header is invalid
  // or input_len (length of the input_data) is shorter than expected



uint16_t calculate_checksum(uint8_t* input_data, size_t input_len) {

  // TODO
  // Calculate a checksum over input_data
  // Return the checksum value

 uint16_t checksum = 0x0000;
  for (int i = 0; i < input_len; i++) {
    checksum = checksum + input_data[i];
  }
  return checksum;

  // set is_checksum = true
}

uint16_t lfsr_step(uint16_t oldstate) {

  // TODO
  // Calculate the new LFSR state given previous state
  // Return the new LFSR state

 uint16_t s_old = oldstate>>1;
  uint16_t old_6 = (oldstate<<9) & 0x8000; 
  uint16_t old_0 = (oldstate<<15)& 0x8000;
  uint16_t old_13 =(oldstate<<2) & 0x8000;
  uint16_t old_9 = (oldstate <<6) & 0x8000;
  uint16_t new_sig = old_0^old_6^old_9^old_13;
  uint16_t newstate = s_old | new_sig;
  return newstate;
  
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        


void decrypt_data(uint8_t* input_data, size_t input_len,
                  uint8_t* output_data, size_t output_len,
                  uint16_t encryption_key) {

    uint16_t step = lfsr_step(encryption_key);
    // a loop that iterates over the input data in pairs of bytes
    for (size_t i = 0; i+1< input_len; i +=2){
      //Initializes a variable byte with the current value of step.
      uint8_t byte = step;
      //Initializes a variable byte2 with the upper byte of the current value of step.
      uint8_t byte2 = step >> 8;
      output_data[i] = byte ^ input_data[i];
      output_data[i+1] = byte2 ^ input_data[i+1];
      step = lfsr_step(step);
    }
    if (input_len % 2 != 0){
      //If the input data length is odd, performs XOR between the 
      //last byte of input_data and the current step value and stores
      // the result in the last position of output_data.
      output_data[input_len -1 ] = step ^ input_data[input_len - 1];
    }


  // TODO
  // Decrypt input_data and write result to output_data
  // Uses lfsr_step() to calculate psuedorandom numbers, initialized with encryption_key
  // Step the LFSR once before encrypting data
  // Apply psuedorandom number with an XOR in little-endian order
  // Beware: input_data may be an odd number of bytes

}

size_t decompress_data(uint8_t* input_data, size_t input_len,
                       uint8_t* output_data, size_t output_len,
                       uint8_t* dictionary_data) {

  // TODO
  // Decompress input_data and write result to output_data
  // Return the length of the decompressed data
int j = 0;
int i = 0;
while (i < input_len) {
  if (input_data[i] == 0x07) {
    if (i == input_len - 1) {
      if (j < output_len) {
        output_data[j] = 0x07;
        return j + 1;
        }
      else {
        return output_len;
          }
        }
    if (input_data[i+1] == 0x00) {
      if(j<output_len) {
        output_data[j] = 0x07;
        j++;
        i+=2;
      }

      else{
        return output_len;
        }
      }
    else{
      uint8_t h1 = (input_data[i+1] & 0xF0) >>4;
      uint8_t h2 = (input_data[i+1]  & 0x0F);
      int current_place = j;
      for (int k = j; k < h1+current_place; k++){
        if(j<output_len){
          output_data[k] = dictionary_data[h2];
          j++;
        }
        else {
          return output_len;
        }
        }
        i +=2;
      }                  
     }
     else{
            if (j<output_len){
                output_data[j] = input_data[i];
                j++;
                i++;
            }
        }
     }
     return j;
     }




