require 'spec_helper'

describe LuaTable do
  it 'has a version number' do
    expect(LuaTable::VERSION).not_to be nil
  end
  describe 'parse' do
    it 'parse assignations' do
      expect(LuaTable.parse("bouh = 23")).to eq({"bouh" => 23})
    end
    it 'parse key assignations' do
      expect(LuaTable.parse('["bouh"] = 23')).to eq({"bouh" => 23})
    end

    it "ignore comment" do
      expect(LuaTable.parse("bouh = 23 --comment\n")).to eq({"bouh" => 23})
    end

    it 'work without spaces' do 
      expect(LuaTable.parse('["bouh"]=23')).to eq({"bouh" => 23})
    end

    it 'parse table with nil values' do
      expect(LuaTable.parse('a = { 1, nil, 3,}')).to eq({"a" => { 1 => 1, 2 => nil, 3 => 3}})
    end

    it 'parse consecutive assignements' do
      expect(LuaTable.parse("a=1\n b=2")).to eq({'a' => 1, 'b' => 2})
    end

    it "parse table without key" do
      expect(LuaTable.parse('bouh = {1, 2, 3,}')).to eq({"bouh" => { 1=> 1, 2 => 2, 3 => 3 }})
    end

    it "parse table with integer key" do
      expect(LuaTable.parse('bouh = { [123] = 23, }')).to eq ({"bouh" => { 123 => 23  }})
    end

    it "parse escaped character" do
      expect(LuaTable.parse('v = "\""')['v']).to eq "\""
    end

    it "parse table with key" do
      expect(LuaTable.parse('bouh= { ["a"] = 1, ["b"] = 2, ["c"] = 3,}')).to eq({"bouh" =>{'a' => 1, 'b' => 2, 'c'=> 3}})
    end

    it "parse table with key / or without" do
      expect(LuaTable.parse('bouh= { "1", ["b"] = 2, ["c"] = 3, 4,}')).to eq({"bouh" =>{1 => '1', 'b' => 2, 'c'=> 3, 2 => 4}})
    end

    it "parse table recursively" do
      expect(LuaTable.parse('bouh= {"a", ["b"] = {1, 2, 3,}, "c",}')).to eq({"bouh" => {1 => 'a', 'b' => {1 => 1, 2 => 2, 3 => 3}, 2 => 'c'}})
    end

    it 'parse integer' do
      expect(LuaTable.parse("v = 23")['v']).to eq(23)
    end

    it 'pare float' do
      expect(LuaTable.parse("v = 1.34")["v"]).to eq 1.34
    end

    it 'parse null' do
    	expect(LuaTable.parse('v = nil')['v']).to be_nil
    end
    it 'parse true' do
    	expect(LuaTable.parse('v = true')['v']).to eq true
    end
    it 'parse false' do
    	expect(LuaTable.parse('v = false')['v']).to eq false
    end
    it 'parse NaN' do
      expect(LuaTable.parse('v = NaN')['v']).to be_nan
    end
    it 'parse strings' do
      expect(LuaTable.parse('v = "Léh"')['v']).to eq 'Léh'
    end
  end
end
