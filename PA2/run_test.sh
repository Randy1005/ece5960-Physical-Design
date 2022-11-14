./build/main/fp 0.5 input_pa2/1.block input_pa2/1.nets output/1.out
./build/main/fp 0.5 input_pa2/2.block input_pa2/2.nets output/2.out
./build/main/fp 0.5 input_pa2/3.block input_pa2/3.nets output/3.out
./build/main/fp 0.5 input_pa2/ami33.block input_pa2/ami33.nets output/ami33.out
./build/main/fp 0.5 input_pa2/ami49.block input_pa2/ami49.nets output/ami49.out
./build/main/fp 0.5 input_pa2/xerox.block input_pa2/xerox.nets output/xerox.out
./build/main/fp 0.5 input_pa2/hp.block input_pa2/hp.nets output/hp.out

# run checker
./checker/checker.py 1 output/1.out
./checker/checker.py 2 output/2.out
./checker/checker.py 3 output/3.out
./checker/checker.py ami33 output/ami33.out
./checker/checker.py ami49 output/ami49.out
./checker/checker.py xerox output/xerox.out
./checker/checker.py hp output/hp.out

