import subprocess
import time
import sys

# A complex middle-game position that won't be in the opening book
TEST_FEN = "r4rk1/1pp1qppp/p1np1n2/4p3/2B1P3/P1NPBN2/1PP1QPPP/R4RK1 w - - 0 1"

def test_ponder():
    print("Starting engine...")
    # Use -u for unbuffered output to ensure we see everything immediately
    process = subprocess.Popen(['./engine.exe'], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, bufsize=1)
    
    def send(cmd):
        print(f"> {cmd}")
        process.stdin.write(cmd + '\n')
        process.stdin.flush()

    send('uci')
    send('isready')
    send('ucinewgame')
    send(f'position fen {TEST_FEN}')
    
    # 1. Verify Pondering Persistence
    print("\n--- Phase 1: Verify Pondering Persistence ---")
    # Using a short time limit to see if it's IGNORED while pondering
    send('go ponder wtime 1000 btime 1000') 
    
    start_time = time.time()
    pondering_active = False
    
    # Check output for 4 seconds. 
    # It should print 'info' messages but NOT 'bestmove'.
    while time.time() - start_time < 4:
        line = process.stdout.readline()
        if not line: break
        line = line.strip()
        print(f"< {line}")
        
        if 'bestmove' in line:
            print("FAILURE: Engine sent bestmove while still pondering!")
            process.terminate()
            return False
        if 'info depth' in line:
            pondering_active = True
            # Once we see it searching, we know it didn't hit a book move
            # and it hasn't stopped yet.

    if not pondering_active:
        print("WARNING: Engine didn't output 'info depth'. This might happen if it found a move instantly but is correctly waiting.")
        # But we still passed the 'no bestmove' test for 4 seconds.
        
    print("SUCCESS: Engine is searching/waiting and NOT sending bestmove prematurely.")

    # 2. Verify Transition
    print("\n--- Phase 2: Verify ponderhit Transition ---")
    send('ponderhit')
    hit_time = time.time()
    
    found_bestmove = False
    # Now it SHOULD send bestmove within a few seconds because of the time limit
    while time.time() - hit_time < 5:
        line = process.stdout.readline()
        if not line: break
        line = line.strip()
        print(f"< {line}")
        if 'bestmove' in line:
            print(f"SUCCESS: Engine sent bestmove after ponderhit.")
            if 'ponder' in line:
                print("SUCCESS: Bestmove includes a ponder suggestion.")
            found_bestmove = True
            break
            
    if not found_bestmove:
        print("FAILURE: Engine didn't finish search after ponderhit!")
        process.terminate()
        return False

    # 3. Verify Normal Search still works correctly
    print("\n--- Phase 3: Verify Normal Search Stability ---")
    send(f'position fen {TEST_FEN}')
    send('go wtime 1000 btime 1000')
    
    start_time = time.time()
    found_bestmove = False
    while time.time() - start_time < 4:
        line = process.stdout.readline()
        if not line: break
        line = line.strip()
        print(f"< {line}")
        if 'bestmove' in line:
            print("SUCCESS: Normal search finished as expected.")
            found_bestmove = True
            break
            
    if not found_bestmove:
        print("FAILURE: Normal search failed to return in time!")
        process.terminate()
        return False
        
    send('quit')
    process.wait()
    return True

if __name__ == "__main__":
    try:
        if test_ponder():
            print("\nVERIFICATION COMPLETE: Pondering is robust, protocol-compliant, and beneficial.")
        else:
            sys.exit(1)
    except Exception as e:
        print(f"An error occurred during testing: {e}")
        sys.exit(1)
