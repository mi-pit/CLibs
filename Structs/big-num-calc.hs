intmax :: Integer
intmax = 2 ^ 64

fromNumberArray :: [Integer] -> Integer
fromNumberArray = fromNumberArrayRec 0

fromNumberArrayRec :: Integer -> [Integer] -> Integer
fromNumberArrayRec _   []     = 0
fromNumberArrayRec pos [x]    = numFromPos pos x
fromNumberArrayRec pos (x:xs) = (numFromPos pos x) + (fromNumberArrayRec (pos + 1) xs)
-- fromNumberArrayRec

numFromPos :: Integer -> Integer -> Integer
numFromPos pos n = (intmax ^ pos) * n
