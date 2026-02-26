# queens.py - 8 queens solver - Maarten Pennings - 2026 Feb 26
def safe(q,qs,n) : 
  match qs :
    case ()      : return True
    case (x,*xs) : return q!=x and abs(q-x)!=n and safe(q,xs,n+1)

def solve(k) :
  match k :
    case 0 : return [()]
    case x : return [ (q,)+qs for qs in solve(x-1) for q in range(8) if safe(q,qs,1)]

for i,s in enumerate(solve(8)) : 
  print(f"Solution {i}\n{'\n'.join([ '. '*x+'Q '+'. '*(7-x) for x in s])}\n")
