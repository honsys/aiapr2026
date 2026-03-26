# Gem Zip Test
# ------------

# 1. Test compression
sys.print("Testing compression...")
success = zip.compress("gem_src", "test_backup.zip")
sys.print("Compression success:", success)

# 2. Test decompression
sys.print("\nTesting decompression...")
success = zip.decompress("test_backup.zip", "test_extract")
sys.print("Decompression success:", success)

# 3. Test password compression
sys.print("\nTesting password-protected compression...")
success = zip.compress("gem_src", "test_secure.zip", "secret123")
sys.print("Secure compression success:", success)

# 4. Test password decompression
sys.print("\nTesting password-protected decompression...")
success = zip.decompress("test_secure.zip", "test_secure_extract", "secret123")
sys.print("Secure decompression success:", success)

# Cleanup
sys.exec("rm test_backup.zip")
sys.exec("rm test_secure.zip")
sys.exec("rm -rf test_extract")
sys.exec("rm -rf test_secure_extract")

sys.print("\nZip tests completed.")
