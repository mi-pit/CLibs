uintmax :: Integer
uintmax = (2 ^ 64) - 1


data Sign = Positive | Negative
    deriving (Show, Eq)

data NumbersArray = NumbersArray {
    sign    :: Sign,
    numbers :: [Integer]
} deriving (Show, Eq)


numFromPos :: Integer -> Integer -> Integer
numFromPos pos n = ((uintmax + 1) ^ pos) * n


fromNumberArray :: NumbersArray -> Integer
fromNumberArray = fromNumberArrayRec 0

fromNumberArrayRec :: Integer -> NumbersArray -> Integer
fromNumberArrayRec _   []     = 0
fromNumberArrayRec pos [x]    = numFromPos pos x
fromNumberArrayRec pos (x:xs) = (numFromPos pos x) + (fromNumberArrayRec (pos + 1) xs)

toNumberArray :: Integer -> NumbersArray
toNumberArray n
  | n == 0    = []
  | otherwise = let digit = n `mod` (uintmax + 1)
                    rest  = n `div` (uintmax + 1)
                in digit : toNumberArray rest
