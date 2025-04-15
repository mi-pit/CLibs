intmax :: Integer
intmax = 10000000000000000000

bigNum :: Integer
bigNum = intmax ^ 0x1000


numFromPos :: Integer -> Integer -> Integer
numFromPos pos n = (intmax ^ pos) * n


fromNumberArray :: [Integer] -> Integer
fromNumberArray = fromNumberArrayRec 0

fromNumberArrayRec :: Integer -> [Integer] -> Integer
fromNumberArrayRec _   []     = 0
fromNumberArrayRec pos [x]    = numFromPos pos x
fromNumberArrayRec pos (x:xs) = (numFromPos pos x) + (fromNumberArrayRec (pos + 1) xs)

toNumberArray :: Integer -> [Integer]
toNumberArray n
  | n == 0    = []
  | otherwise = let digit = n `mod` intmax
                    rest  = n `div` intmax
                in digit : toNumberArray rest
