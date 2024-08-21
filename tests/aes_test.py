import os
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

port = 8550

# Pseudo-random key and initialisation vector
# key = port.to_bytes(32, 'big') #os.urandom(32)           # (32*8=256-bit. AES also accepts 128/192-bit)
# print(f"key: {key}")
# init_vector = port.to_bytes(16, 'big') # os.urandom(16)   # (16*8=128-bit. AES only accepts this size)
# print(f"initialization vector: {init_vector}")

# # Setup module-specific classes
# cipher = Cipher(algorithms.AES(key), modes.GCM(init_vector))
# encryptor = cipher.encryptor()
# decryptor = cipher.decryptor()

# # Encrypt and decrypt data
# chipertext = encryptor.update(b"a secret message") + encryptor.finalize()
# print(chipertext)
# plaintext = decryptor.update(chipertext)
# print(plaintext)

key = port.to_bytes(32, 'big')
init_vector = port.to_bytes(16, 'big')
cipher = Cipher(algorithms.AES(key), modes.GCM(init_vector))

key2 = port.to_bytes(32, 'big')
init_vector2 = port.to_bytes(16, 'big')
cipher2 = Cipher(algorithms.AES(key2), modes.GCM(init_vector2))

encryptor = cipher.encryptor()
decryptor = cipher.decryptor()

encryptor2 = cipher2.encryptor()
decryptor2 = cipher2.decryptor()

chipertext = encryptor.update(b"a secret message") + encryptor.finalize()
chipertext2 = encryptor2.update(b"a secret message") + encryptor2.finalize()

print(chipertext == chipertext2)
plaintext = decryptor.update(chipertext)
print(plaintext)
p = decryptor2.update(chipertext)
print(p)