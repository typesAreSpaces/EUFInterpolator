#include <exception>
#include <thread>
#include <fstream>
#include <iostream>
#include <string>
#define IZ3_PREFIX     "iz3_instance_"
#define MATHSAT_PREFIX "mathsat_instance_"
#define EUFI_PREFIX    "eufi_instance_"
#define SMT_SUFFIX     ".smt2"
#define TXT_SUFFIX     ".txt"

// A-part : f^n(x) = f^{n+1}(x), f^2(x) = x, f(a) \neq a
// B-part : x = a

void iz3_instance(unsigned);
void mathsat_instance(unsigned);
void eufi_instance(unsigned);

void iz3_process(unsigned);
void mathsat_process(unsigned);
void eufi_process_0(unsigned);
void eufi_process_1(unsigned);
void eufi_process_2(unsigned);
void eufi_process_3(unsigned);
void eufi_process_4(unsigned);
void eufi_process_5(unsigned);
void eufi_process_6(unsigned);
void eufi_process_7(unsigned);
void eufi_process_8(unsigned);
void eufi_process_9(unsigned);
void eufi_process_10(unsigned);
void eufi_process_11(unsigned);
void eufi_process_12(unsigned);
void eufi_process_13(unsigned);
void eufi_process_14(unsigned);
void eufi_process_15(unsigned);
void eufi_process_16(unsigned);
void eufi_process_17(unsigned);

int main(){

  unsigned n = 10000;
  //unsigned n = 100;
  //unsigned n = 10;

  //std::thread iz3(iz3_process, n);
  //std::thread mathsat(mathsat_process, n);
  //std::thread eufi_0(eufi_process_0, n);
  //std::thread eufi_1(eufi_process_1, n);
  //std::thread eufi_2(eufi_process_2, n);
  //std::thread eufi_3(eufi_process_3, n);
  //std::thread eufi_4(eufi_process_4, n);
  //std::thread eufi_5(eufi_process_5, n);
  //std::thread eufi_6(eufi_process_6, n);
  //std::thread eufi_7(eufi_process_7, n);
  //std::thread eufi_8(eufi_process_8, n);
  //std::thread eufi_9(eufi_process_9, n);
  //std::thread eufi_10(eufi_process_10, n);
  std::thread eufi_11(eufi_process_11, n);
  std::thread eufi_12(eufi_process_12, n);
  std::thread eufi_13(eufi_process_13, n);
  std::thread eufi_14(eufi_process_14, n);
  std::thread eufi_15(eufi_process_15, n);
  std::thread eufi_16(eufi_process_16, n);
  std::thread eufi_17(eufi_process_17, n);

  //iz3.join();
  //mathsat.join();
  //eufi_0.join();
  //eufi_1.join();
  //eufi_2.join();
  //eufi_3.join();
  //eufi_4.join();
  //eufi_5.join();
  //eufi_6.join();
  //eufi_7.join();
  //eufi_8.join();
  //eufi_9.join();
  //eufi_10.join();
  eufi_11.join();
  eufi_12.join();
  eufi_13.join();
  eufi_14.join();
  eufi_15.join();
  eufi_16.join();
  eufi_17.join();

  return 0;
}

void iz3_instance(unsigned n){
  std::ofstream out(IZ3_PREFIX + std::to_string(n) + SMT_SUFFIX);

  out << "(set-option :produce-interpolants true)" << std::endl;
  out << "(declare-sort A 0)" << std::endl;
  out << "(declare-fun x () A)" << std::endl;
  out << "(declare-fun a () A)" << std::endl;
  out << "(declare-fun f (A) A)" << std::endl;
  out << std::endl;
  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  // ------------------------------------------------------------
  // Instantiation
  out << "(= "; 
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << " ";
  n++;
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << ") ;; Parametrized formula for n = " << --n << std::endl;
  // ------------------------------------------------------------
  out << "(= (f (f x)) x)" << std::endl;
  out << "(distinct (f a) a)" << std::endl;
  out << ") :named a_part" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(assert (!" << std::endl;
  out << "(= x a)" << std::endl;
  out << ":named b_part" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(check-sat)" << std::endl;
  out << "(get-interpolant a_part b_part)" << std::endl;
  out.close();
} 

void mathsat_instance(unsigned n){
  std::ofstream out(MATHSAT_PREFIX + std::to_string(n) + SMT_SUFFIX);

  out << "(set-option :produce-interpolants true)" << std::endl;
  out << "(declare-sort A 0)" << std::endl;
  out << "(declare-fun x () A)" << std::endl;
  out << "(declare-fun a () A)" << std::endl;
  out << "(declare-fun f (A) A)" << std::endl;
  out << std::endl;
  out << "(assert (!" << std::endl;
  out << "(and" << std::endl;
  // ------------------------------------------------------------
  // Instantiation
  out << "(= "; 
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << " ";
  n++;
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << ") ;; Parametrized formula for n = " << --n << std::endl;
  // ------------------------------------------------------------
  out << "(= (f (f x)) x)" << std::endl;
  out << "(distinct (f a) a)" << std::endl;
  out << ") :interpolation-group a_part" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(assert (!" << std::endl;
  out << "(= x a)" << std::endl;
  out << ":interpolation-group b_part" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(check-sat)" << std::endl;
  out << "(get-interpolant (a_part))" << std::endl;
  out << "(exit)" << std::endl;
  out.close();
}

void eufi_instance(unsigned n){
  std::ofstream out(EUFI_PREFIX + std::to_string(n) + SMT_SUFFIX);

  out << "(declare-sort A 0)" << std::endl;
  out << "(declare-fun x () A)" << std::endl;
  out << "(declare-fun a () A)" << std::endl;
  out << "(declare-fun f (A) A)" << std::endl;
  out << std::endl;
  out << "(assert (and" << std::endl;
  // ------------------------------------------------------------
  // Instantiation
  out << "(= "; 
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << " ";
  n++;
  for(unsigned i = 0; i < n; ++i)
    out << "(f ";
  out << "x";
  for(unsigned i = 0; i < n; ++i)
    out << ")";
  out << ") ;; Parametrized formula for n = " << --n << std::endl;
  // ------------------------------------------------------------
  out << "(= (f (f x)) x)" << std::endl;
  out << "(distinct (f a) a)" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(assert (and" << std::endl;
  out << "(= x a)" << std::endl;
  out << "))" << std::endl;
  out << std::endl;
  out << "(check-sat)" << std::endl;
  out.close();
}

void iz3_process(unsigned n){
  system("rm -rf iz3_results.txt");
  for(unsigned i = 1; i < n; ++i){
    iz3_instance(i);
    system(("echo \"test: " + std::to_string(i) + "\">> iz3_results.txt").c_str());
    system(("{ time ../../bin/z3-interp  " + (IZ3_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> iz3_results.txt").c_str());
    system(("rm " + (IZ3_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
  }
}

void mathsat_process(unsigned n){
  system("rm -rf mathsat_results.txt");
  for(unsigned i = 1; i < n; ++i){
    mathsat_instance(i);
    system(("echo \"test: " + std::to_string(i) + "\">> mathsat_results.txt").c_str());
    system(("{ time ../../bin/mathsat " + (MATHSAT_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> mathsat_results.txt").c_str());
    system(("rm " + (MATHSAT_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
  }
}

void eufi_process_0(unsigned n){
  system("rm -rf eufi_results_0.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 0){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_0.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_0.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_1(unsigned n){
  system("rm -rf eufi_results_1.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 1){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_1.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_1.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_2(unsigned n){
  system("rm -rf eufi_results_2.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 2){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_2.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_2.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_3(unsigned n){
  system("rm -rf eufi_results_3.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 3){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_3.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_3.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_4(unsigned n){
  system("rm -rf eufi_results_4.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 4){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_4.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_4.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_5(unsigned n){
  system("rm -rf eufi_results_5.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 5){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_5.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_5.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_6(unsigned n){
  system("rm -rf eufi_results_6.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 6){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_6.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_6.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_7(unsigned n){
  system("rm -rf eufi_results_7.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 7){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_7.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_7.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_8(unsigned n){
  system("rm -rf eufi_results_8.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 8){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_8.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_8.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_9(unsigned n){
  system("rm -rf eufi_results_9.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 9){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_9.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_9.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_10(unsigned n){
  system("rm -rf eufi_results_10.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 10){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_10.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_10.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_11(unsigned n){
  system("rm -rf eufi_results_11.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 11){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_11.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_11.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_12(unsigned n){
  system("rm -rf eufi_results_12.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 12){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_12.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_12.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_13(unsigned n){
  system("rm -rf eufi_results_13.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 13){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_13.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_13.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_14(unsigned n){
  system("rm -rf eufi_results_14.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 14){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_14.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_14.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_15(unsigned n){
  system("rm -rf eufi_results_15.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 15){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_15.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_15.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_16(unsigned n){
  system("rm -rf eufi_results_16.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 16){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_16.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_16.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}

void eufi_process_17(unsigned n){
  system("rm -rf eufi_results_17.txt");
  for(unsigned i = 6000; i < n; ++i){
    unsigned r = i % 18;
    if(r == 17){
      eufi_instance(i);
      system(("echo \"test: " + std::to_string(i) + "\">> eufi_results_17.txt").c_str());
      system(("{ time ./bin/eufi " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX + "; } 2>> eufi_results_17.txt").c_str());
      system(("rm " + (EUFI_PREFIX + std::to_string(i)) + SMT_SUFFIX).c_str());
    }
  }
}
