#!/usr/bin/env luajit

local sha = require "sha2"
local lfs = require "lfs"

local MAX_FILES = 200000
local BLOCK_SIZE = 32768


--
-- FileMetadata class
--

FileMetadata = {}

function FileMetadata:new(path)
	local t = setmetatable({}, { __index = FileMetadata })

	t.path = path
	t.checksum = false
	return t
end

function FileMetadata:get_path()
	return self.path
end

function FileMetadata:compute_sha256sum()
	local f = assert(io.open(self.path, "rb"))
	local add_to_sha = sha.sha256()
	local data = f:read(BLOCK_SIZE)
	while data do
		add_to_sha(data)
		data = f:read(BLOCK_SIZE)
	end
	self.checksum = add_to_sha()
end

function FileMetadata:get_sha256sum()
	return self.checksum
end


-- 
-- DirTree class
--

DirTree = {}

function DirTree:new(root_dir)
	local t = setmetatable({}, { __index = DirTree })
	t.root_dir = root_dir
	return t
end

function DirTree:get_file_list()
	-- https://keplerproject.github.io/luafilesystem/examples.html
	local dirs = { self.root_dir }
	local files = {}

	while (#dirs > 0) and (#files < MAX_FILES) do
		local current_dir = table.remove(dirs)
    	for file in lfs.dir(current_dir) do
			if file ~= "." and file ~= ".." then
				local f = current_dir .. '/' .. file
				local attr = lfs.attributes(f)
				assert (type(attr) == "table")
				if attr.mode == "directory" then
					table.insert(dirs, f)
				elseif attr.mode == "file" then
					table.insert(files, FileMetadata:new(f))
					if #files % 1000 == 0 then
						print("Found " .. #files .. " files")
					end
				end
            end
        end
    end

	return files
end


--
-- FileSet class
--

FileSet = { self_checks = 0 }

function FileSet:new(tag)
	local t = setmetatable({}, { __index = FileSet })
	t.tag = tag
	t.files = {}
	return t
end

function FileSet:get_tag()
	return self.tag
end

function FileSet:add_file(file)
	table.insert(self.files, file)
end

function FileSet:get_files()
	return self.files
end

function FileSet:is_ok()
	for _,f in pairs(self.files) do
		FileSet.self_checks = FileSet.self_checks + 1
		c = f:get_sha256sum()
		if self.tag ~= c then
			print("Checksum for " .. f:get_path() .. ' ' .. 
			       f:get_sha256sum() .. " doesn't match tag " ..
				   self.tag)
			return false
		end
	end
	return true
end


--
-- FileSets class
-- 

FileSets = { self_checks = 0 }

function FileSets:new()
	local t = setmetatable({}, { __index = FileSets })
	t.sets = {}
	return t
end

function FileSets:add_file(file)
	local c = file:get_sha256sum()
	if self.sets[c] == nil then
		self.sets[c] = FileSet:new(c)
	end
	self.sets[c]:add_file(file)
end

function FileSets:is_ok()
	for k,v in self.sets do
		if k ~= v:get_tag() then
			print("Table key " .. k .. " doesn't match FileSet tag " ..
			       v.get_tag())
			return false
		end
		if v:is_ok() == false then
			return false
		end
	end
	return true
end

dir_tree = DirTree:new("/run/media/bwh/Samsung_T5/Acer/Videos")
file_list = dir_tree:get_file_list()
for _,f in pairs(file_list) do
	f:compute_sha256sum()
end

for _,f in pairs(file_list) do
 	print(f:get_sha256sum() .. ' ' .. f:get_path())
end

