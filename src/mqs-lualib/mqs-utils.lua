local M = {}

local ffi = require("ffi")
ffi.cdef[[
int mkdir(const char *dir, int mode);
char *strerror(int errnum);
]]

local function errno()
      return ffi.string(ffi.C.strerror(ffi.errno()))
end

local function mkdir(dir, mode)
      return ffi.C.mkdir(dir, mode)
end

function is_dir(dir)
   local fp, err = io.open(dir, "r")
   if fp ~= nil then
      io.close(fp)
      return true
   else
      if string.match(err,"No such file or directory") then
	 return false
      else
	 return true
      end
   end
end

function parse_csv_line(line,sep) 
	local res = {}
	local pos = 1
	sep = sep or ','
	while true do 
		local c = string.sub(line,pos,pos)
		if (c == "") then break end
		if (c == '"') then
			-- quoted value (ignore separator within)
			local txt = ""
			repeat
				local startp,endp = string.find(line,'^%b""',pos)
				txt = txt..string.sub(line,startp+1,endp-1)
				pos = endp + 1
				c = string.sub(line,pos,pos) 
				if (c == '"') then txt = txt..'"' end 
				-- check first char AFTER quoted string, if it is another
				-- quoted string without separator, then append it
				-- this is the way to "escape" the quote char in a quote. example:
				--   value1,"blub""blip""boing",value3  will result in blub"blip"boing  for the middle
			until (c ~= '"')
			table.insert(res,txt)
			assert(c == sep or c == "")
			pos = pos + 1
		else	
			-- no quotes used, just look for the first separator
			local startp,endp = string.find(line,sep,pos)
			if (startp) then 
				table.insert(res,string.sub(line,pos,startp-1))
				pos = endp + 1
			else
				-- no separator found -> use rest of string and terminate
				table.insert(res,string.sub(line,pos))
				break
			end 
		end
	end

	-- Remove heading whitespace if there is one
	for k,v in ipairs(res) do
	   res[k] = v:gsub("^%s*", "")	   
	end
	-- Remove Single quotes if there one
	for k,v in ipairs(res) do
	   newv = v:gsub("^'", "")
	   if newv ~= v then
	      res[k] = newv:sub(1, -2)
	   end
	end

	
	return res
end


M.errno = errno
M.mkdir = mkdir
M.is_dir = is_dir
M.parse_csv_line = parse_csv_line

return M
