#!/bin/bash
print_header() {
 echo " GST BILL "
 echo "=============================="
 echo "Date: $(date)"
 echo "Customer Name: $1"
 echo "------------------------------"
}
calculate_bill() {
 total=0
 total_gst=0
 for (( i=0; i<${#prices[@]}; i++ ))
 do
 total=$(( total + prices[i] ))
 item_gst=$(( prices[i] * gst_rates[i] / 100 ))
 total_gst=$(( total_gst + item_gst ))
 done
 final_amount=$(( total + total_gst ))
}
display_bill() {
 echo "Items Purchased:"
 for (( i=0; i<${#items[@]}; i++ ))
 do
 echo "${items[i]} - ₹${prices[i]} - GST ${gst_rates[i]}%"
 done
 echo "------------------------------"
 echo "Total Amount: ₹$total"
 echo "Total GST: ₹$total_gst"
 echo "Final Amount: ₹$final_amount"
}
if [ $# -eq 0 ]
then
 echo "Usage: ./gst_bill.sh CustomerName"
 exit 1
fi
customer_name=$1
uppercase_name=$(echo $customer_name | tr 'a-z' 'A-Z')
length=${#customer_name}
echo "Customer Name in Uppercase: $uppercase_name"
echo "Length of Name: $length"
declare -a items
declare -a prices
declare -a gst_rates
echo "Enter number of items:"
read n
for (( i=0; i<n; i++ ))
do
 echo "Enter item name:"
 read item
 items[i]=$item
 echo "Enter price:"
 read price
 prices[i]=$price
 echo "Enter GST % for this item:"
 read gst_input
 gst_rates[i]=$gst_input
done
print_header "$customer_name"
calculate_bill
display_bill
