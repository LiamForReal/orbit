def mod_exp(base: int, power: int, mod: int) -> int:
    """Function that implements modular exponentiation in very efficient way 

    Args:
        base (int): base value
        power (int): power value of the base
        mod (int): modulo value

    Returns:
        int: result value of (base ^ power) MOD mod
    """
    
    result = 1
    
    base = base % mod
    
    if (0 == base):
        return 0
    
    while (power > 0):
        if (1 == (power & 1)):
            result = (result * base) % mod
        
        power = power >> 1
        base = (base ** 2) % mod
    
    return result