require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.scalar" do
  
  before(:each) do
    @side = 3
    @value = 8
    @a = Matrix.scalar(@side, @value)
  end
  
  it "returns a Matrix" do
    @a.class.should == Matrix
  end
  
  it "returns a n x n matrix" do
    @a.row_size.should == @side
    @a.column_size.should == @side
  end
  
  it "initializes diagonal to value" do
    (0...@a.row_size).each do |i|
      @a[i, i].should == @value
    end
  end
  
  it "initializes all non-diagonal values to 0" do
    (0...@a.row_size).each do |i|
      (0...@a.column_size).each do |j|
        if i != j
          @a[i, j].should == 0
        end
      end
    end
  end
end
