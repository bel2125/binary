print("Test for binary")

-- Define some helper functions for the test
dcprint = print

function deepcompare(t1, t2)
  if (type(t1) ~= type(t2)) then
     dcprint("deepcompare: type mismatch")
     return false;
  end
  if (type(t1) ~= "table") then
     return (t1 == t2);
  end
  if (#t1 ~= #t2) then
     dcprint("deepcompare: different table length (" .. #t1 .. " <> " .. #t2 .. ")")
     return false;
  end
  for k,v in pairs(t1) do
     local v2 = t2[k]
     if not deepcompare(v, v2) then
       dcprint("deepcompare: subtable not equal " .. k .. " (" .. tostring(v) .. " <> " .. tostring(v2) .. ")")
       return false;
     end
  end
  for k,v in pairs(t2) do
     local v1 = t1[k]
     if not deepcompare(v, v1) then
       dcprint("deepcompare: subtable not equal " .. tostring(k))
       return false;
     end
  end
  return true
end

function asserteq(v1, v2, txt)
  if not deepcompare(v1, v2) then
    --assert(false, txt .. " (" .. tostring(v1) .. " <> " .. tostring(v2) .. ")")
    print("ERROR:", txt .. " (" .. tostring(v1) .. " <> " .. tostring(v2) .. ")")
  end
end


-- First test if all functions are available
asserteq(type(binary), "table", "binary not available");

asserteq(type(binary.band), "function", "binary.band not available");
asserteq(type(binary.bnand), "function", "binary.bnand not available");
asserteq(type(binary.bor), "function", "binary.bor not available");
asserteq(type(binary.bnor), "function", "binary.bnor not available");
asserteq(type(binary.bxor), "function", "binary.bxor not available");
asserteq(type(binary.beq), "function", "binary.beq not available");
asserteq(type(binary.bnot), "function", "binary.bnot not available");

asserteq(type(binary.bget), "function", "binary.bget not available");
asserteq(type(binary.poke), "function", "binary.poke not available");

asserteq(type(binary.toarray), "function", "binary.toarray not available");
asserteq(type(binary.fromarray), "function", "binary.fromarray not available");

print("All functions available")


-- Test logic oparator with one arguments
for i=0,32 do
  asserteq(binary.bnot(0, i), (2^i)-1, "bnot failed")
end

asserteq(binary.bnot(5, 3), 2, "bnot failed")
asserteq(binary.bnot(5, 4), 2+8, "bnot failed")

print("Basic binary locic functions tested (NOT)")


-- Test logic oparator with two arguments
asserteq(binary.band(5, 3), 1, "band failed")
asserteq(binary.bor(5, 3), 7, "bor failed")
asserteq(binary.bxor(5, 3), 6, "bxor failed")

asserteq(binary.band(5, 3, 3), 1, "band failed")
asserteq(binary.bor(5, 3, 3), 7, "bor failed")
asserteq(binary.bxor(5, 3, 3), 6, "bxor failed")
asserteq(binary.band(5, 3, 4), 1, "band failed")
asserteq(binary.bor(5, 3, 4), 7, "bor failed")
asserteq(binary.bxor(5, 3, 4), 6, "bxor failed")

asserteq(binary.bnand(5, 3, 3), 6, "bnand failed")
asserteq(binary.bnor(5, 3, 3), 0, "bnor failed")
asserteq(binary.beq(5, 3, 3), 1, "beq failed")
asserteq(binary.bnand(5, 3, 4), 6+8, "bnand failed")
asserteq(binary.bnor(5, 3, 4), 0+8, "bnor failed")
asserteq(binary.beq(5, 3, 4), 1+8, "beq failed")

print("Basic binary locic functions tested (AND, OR, XOR, NAND, NOR, EQ)")


-- Test bget
for i=0,8 do
  asserteq(binary.bget("\x00", 0, i), 0, "bget(0, "..i..") failed")
end
for i=0,32 do
  asserteq(binary.bget("\x00\x00\x00\x00", 0, i), 0, "bget(0, "..i..") failed")
end

for i=0,32 do
  asserteq(binary.bget("\xFF\xFF\xFF\xFF", 0, i), (2^i)-1, "bget(0, "..i..") failed")
end

--little endian
for i=1,32 do
  asserteq(binary.bget("\x01\x00\x00\x00", 0, i), 1, "bget(0, "..i..") failed")
end

for i=1,32 do
  asserteq(binary.bget("\x10\x00\x00\x00\xF0", 4, i), 1, "bget(4, "..i..") failed")
end

print("bget tested")


-- Test toarray/fromarray for bits
strdata = "\x68\x95";
bitarr = {false, true, true, false, true, false, false, false,
          true, false, false, true, false, true, false, true,
          };
bitnumarr = {0, 1, 1, 0, 1, 0, 0, 0,
             1, 0, 0, 1, 0, 1, 0, 1,
             };
bytearr = {64+32+8, 128+16+4+1};
nibblearr = {8, 6, 5, 9};


str = binary.toarray(strdata, 8)
asserteq(str, bytearr, "toarray (8) failed");


print(str, type(str), #str);
for k,v in pairs(str) do print(k,v,bytearr[k]) end


str = binary.toarray(strdata, 0)
asserteq(str, bitarr, "toarray (0) failed");

print(str, type(str), #str);
for k,v in pairs(str) do print(k,v,bitarr[k]) end


str = binary.toarray(strdata, 1)
asserteq(str, bitnumarr, "toarray (0) failed");

print(str, type(str), #str);
for k,v in pairs(str) do print(k,v,bitnumarr[k]) end



str = binary.toarray(strdata, 4)
asserteq(str, nibblearr, "toarray (4) failed");

print(str, type(str), #str);
for k,v in pairs(str) do print(k,v,nibblearr[k]) end
