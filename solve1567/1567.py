# Python 3.7 or higher
# 2020 April 16, Maarten Pennings
import itertools as it
import fractions as f

# Apply operator `op` to `f1` and `f2` (which are None after div-by-0)
def combine(f1,op,f2):
    if f1==None or f2==None: return None
    if op=='+': return f1+f2
    if op=='-': return f1-f2
    if op=='*': return f1*f2
    if op=='/' and f2!=0: return f1/f2
    return None

# Apply the operations in `ops` on numbers in `nums` in order `opnums`
# nums[0] ops[0] nums[1] ops[1] nums[2] ops[2] nums[3]
def compute(nums,ops,opnums):
    while opnums:
        opnum= opnums.pop(0) 
        res= combine(nums[opnum],ops[opnum],nums[opnum+1])
        nums= nums[:opnum] + [res] + nums[opnum+2:]
        ops= ops[:opnum] + ops[opnum+1:]
        opnums= [ ix - (ix>opnum) for ix in opnums ]
    return res

numbers= [ f.Fraction(1), f.Fraction(5), f.Fraction(6), f.Fraction(7) ]
target=21
#numbers= [ f.Fraction(6), f.Fraction(10), f.Fraction(25), f.Fraction(75), f.Fraction(5), f.Fraction(50)];
#target=728
for nums in it.permutations(numbers) :
    for ops in it.product('+-*/', repeat=len(numbers)-1 ) :
        for opnums in it.permutations( range(len(numbers)-1) ) :
            result= compute(list(nums),list(ops),list(opnums))
            if result==target: 
                lnums,lops,lopnums= list(nums),list(ops),list(opnums)
                while lopnums:
                    opnum= lopnums.pop(0)
                    res= f"({lnums[opnum]}{lops[opnum]}{lnums[opnum+1]})"
                    lnums= lnums[:opnum] + [res] + lnums[opnum+2:]
                    lops= lops[:opnum] + lops[opnum+1:]
                    lopnums= [ ix - (ix>opnum) for ix in lopnums ]
                print( f"{res[1:-1]} = {result}")
