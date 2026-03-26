sys.exec("echo '_temp_val = 12.34' > /tmp/test_val.g")
use "/tmp/test_val.g"
sys.print("Loaded Value:", _temp_val)
