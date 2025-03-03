// Copyright (c) 2020 University of Florida
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Greg Stitt
// University of Florida
//
// This example demonstrates an AFU wrapper class built around the OPAE API 
// to do the following:
// 1) request an FPGA with a specific AFU
// 2) read and write from a memory-mapped register in the FPGA 

#include <cstdlib>
#include <iostream>

#include <opae/utils.h>

#include "AFU.h"

using namespace std;

// Auto-generated by OPAE's afu_json_mgr script
#include "afu_json_info.h"

//=========================================================
// Define the address of the memory-mapped register according the address
// that was used in the RTL code.
//
// NOTE: Ideally this could be generated with a .json file just like the
// AFU_ACCEL_UUID. Without auto-generation, you must manually ensure that
// the addresses match between the RTL code and software code.
//=========================================================
#define USER_REG_ADDR 0x0020


int main(int argc, char *argv[]) {

  try {
    // Create an AFU object to provide basic services for the FPGA. The 
    // constructor searchers available FPGAs for one with an AFU with the
    // the specified ID
    AFU afu(AFU_ACCEL_UUID);
    
    // Test 100 different writes and reads to the user MMIO register.
    unsigned errors = 0;
    for (uint64_t i=0; i < 100; i++) {
      afu.write(USER_REG_ADDR, i);
      uint64_t result = afu.read(USER_REG_ADDR);

      
      if(i < 8) {
        if (result != 0) {
	      cerr << "ERROR: Not reset properly. Read from MMIO register has incorrect value " << result << " instead of " << 0 << endl;
	      errors ++;
        }
      } else if (result != i - 7) {
	      cerr << "ERROR: Read from MMIO register has incorrect value " << result << " instead of " << i - 8 << endl;
	      errors ++;
      }
    }

    if (errors == 0) {
      cout << "All MMIO tests succeeded." << endl;
      return EXIT_SUCCESS;
    }
    else {
      cout << "MMIO tests failed." << endl;
      return EXIT_FAILURE;
    }
  }
  // Exception handling for all the runtime errors that can occur within 
  // the AFU wrapper class.
  catch (const fpga_result& e) {    
    
    // Provide more meaningful error messages for each exception.
    if (e == FPGA_BUSY) {
      cerr << "ERROR: All FPGAs busy." << endl;
    }
    else if (e == FPGA_NOT_FOUND) { 
      cerr << "ERROR: FPGA with accelerator " << AFU_ACCEL_UUID 
	   << " not found." << endl;
    }
    else {
      // Print the default error string for the remaining fpga_result types.
      cerr << "ERROR: " << fpgaErrStr(e) << endl;    
    }
  }
  catch (const runtime_error& e) {    
    cerr << e.what() << endl;
  }
  catch (const opae::fpga::types::no_driver& e) {
    cerr << "ERROR: No FPGA driver found." << endl;
  }

  return EXIT_FAILURE;
}
