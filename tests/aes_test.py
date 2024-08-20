import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

# Pseudo-random key and initialisation vector
key = os.urandom(32)           # (32*8=256-bit. AES also accepts 128/192-bit)
print(f"key: {key}")
init_vector = os.urandom(16)   # (16*8=128-bit. AES only accepts this size)
print(f"initialization vector: {init_vector}")

# Setup module-specific classes
cipher = Cipher(algorithms.AES(key), modes.GCM(init_vector))
encryptor = cipher.encryptor()
decryptor = cipher.decryptor()

# Encrypt and decrypt data
chipertext = encryptor.update(b"a secret message") + encryptor.finalize()
print(chipertext)
plaintext = decryptor.update(chipertext)
print(plaintext)